#include "../src/sio_client.h"
#include "UUID_generator.h"
#include "irb01.h"

#include <boost/tokenizer.hpp>
#include <functional>
#include <sstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#ifdef WIN32
#define HIGHLIGHT(__O__) std::cout<<__O__<<std::endl
#define EM(__O__) std::cout<<__O__<<std::endl

#include <stdio.h>
#include <tchar.h>
#define MAIN_FUNC int _tmain(int argc, _TCHAR* argv[])
#else
#define HIGHLIGHT(__O__) std::cout<<"\e[1;31m"<<__O__<<"\e[0m"<<std::endl
#define EM(__O__) std::cout<<"\e[1;30;1m"<<__O__<<"\e[0m"<<std::endl

#define MAIN_FUNC int main(int argc ,const char* args[])
#endif

using namespace sio;
using namespace std;
std::mutex _lock;

std::condition_variable_any _cond;
bool connect_finish = false;

class connection_listener
{
    sio::client &handler;

public:
    
    connection_listener(sio::client& h):
    handler(h)
    {
    }
    

    void on_connected()
    {
        _lock.lock();
        _cond.notify_all();
        connect_finish = true;
        _lock.unlock();
    }
    void on_close(client::close_reason const& reason)
    {
        std::cout<<"sio closed "<<std::endl;
        exit(0);
    }
    
    void on_fail()
    {
        std::cout<<"sio failed "<<std::endl;
        exit(0);
    }
};

int participants = -1;

socket::ptr current_socket;

string uuid;
string room;

Irb01 player;

MJCard drawTile;
MJCard throwTile;

void bind_events()
{
	current_socket->on("readyToStart", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		room = data[0]->get_string();
		message::list list = message::list();
		list.push(uuid);
		list.push(room);
		current_socket->emit("auth",  list);
		current_socket->emit("ready", list, [](message::list const& ack) {
			_lock.lock();
			player.id = ack[0]->get_int();
			_lock.unlock();
		});
		_lock.unlock();
	}));

	current_socket->on("broadcastOpenDoor", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		// player.opendoorIdx = data->get_int();
		_lock.unlock();
	}));

	current_socket->on("broadcastWindAndRound", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		// player.windIdx  = data->get_vector()[0]->get_int();
		// player.roundIdx = data->get_vector()[1]->get_int();
		_lock.unlock();
	}));

	current_socket->on("dealTile", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		for (int i = 0; i < 5; i++) {
			player.Hand[i] = 0;
			player.discards[i] = 0;
		}
		vector<message::ptr> list = data[0]->get_vector();
		for (message::ptr t : list) {
			player.Hand += MJCard::StringToCard(t->get_string());
		}
		_lock.unlock();
	}));

	current_socket->on("BroadcastBuHua", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		// player.flower +=
		_lock.unlock();
	}));

	current_socket->on("BroadcastHua", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		/*string id     = data[0]->get_string();
		string flower = data->get_vector()[1]->get_string();
		if (id == player.id) {
		    player.hand   -= 
		    player.flower +=
		}*/
		_lock.unlock();
	}));
	
	current_socket->on("draw", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		drawTile = MJCard::StringToCard(data[0]->get_string());
		_lock.unlock();
	}));

	current_socket->on("throw", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		message::list list = message::list();
		cout << "throw: ";
		string tile = player.Throw(drawTile).toString();
		cout << tile << endl;
		list.push(tile);
		current_socket->emit("throwTile", list);
		_lock.unlock();
	}));

	current_socket->on("broadcastThrow", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		throwTile = MJCard::StringToCard(data[1]->get_string());
		player.discards += throwTile;
		if (data[0]->get_int() == player.id) {
			player.Hand -= throwTile;
		}
		_lock.unlock();
	}));

	current_socket->on("command", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		map<string, message::ptr> tmp = data[0]->get_map();
		int command = data[1]->get_int();
		int idx = data[2]->get_int();
		map<int, vector<MJCard>> commandTile = map<int, vector<MJCard>>();
		for (map<string, message::ptr>::iterator it = tmp.begin(); it != tmp.end(); it++) {
			vector<MJCard> cards = vector<MJCard>();
			vector<message::ptr> value = it->second->get_vector();
			for (auto i : value) {
				cards.push_back(MJCard::StringToCard(i->get_string()));
			}
			commandTile[stoi(it->first)] = cards;
		}
		bool canZimo, canOnPonGon, canHu, canEat, canGon, canPon;
		canZimo = (command & COMMAND_ZIMO) != 0;
		canOnPonGon = (command & (COMMAND_ANGON | COMMAND_PONGON)) != 0;
		canHu  = (command & COMMAND_HU) != 0;
		canEat = (command & COMMAND_EAT) != 0;
		canGon = (command & COMMAND_GON) != 0;
		canPon = (command & COMMAND_PON) != 0;

		pair<CommandType, MJCard> cmd;
		if (canZimo || canOnPonGon) {
			MJCard gonTile;
			if ((command & COMMAND_ANGON) != 0) {
				gonTile = commandTile[COMMAND_ANGON][0];
			}
			else if ((command & COMMAND_PONGON) != 0) {
				gonTile = commandTile[COMMAND_PONGON][0];
			}
			else {
				gonTile = MJCard();
			}
			cmd = player.WannaHuGon(canZimo, canOnPonGon, drawTile, gonTile);
		}
		else {
			cmd = pair<CommandType, MJCard>();
			cmd.first  = player.WannaHGPE(canHu, canGon, canPon, canEat, throwTile, idx);
			cmd.second = throwTile;
			if (cmd.first == COMMAND_EAT) {
				int eat = player.Pick2Eat(throwTile);
				if (eat == 0) {
					cmd.second.value -= 2;
				}
				else if (eat == 1) {
					cmd.second.value--;
				}
			}
		}
		stringstream ss;
		ss << "{Command:" << cmd.first << ",Tile:" << cmd.second.toString() << ",Score:0}";
		string returnStr;
		ss >> returnStr;
		message::list list = message::list();
		list.push(returnStr);
		current_socket->emit("sendCommand", list);
		_lock.unlock();
	}));

	current_socket->on("BroadcastCommand", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		int command = data[1]->get_int();
		string tileStr = data[2]->get_string();
		if (command == COMMAND_EAT) {
			boost::tokenizer<> tok(tileStr);
			boost::tokenizer<>::iterator it = tok.begin();
			MJCard firstTile = MJCard::StringToCard(*it);
			MJCard centerTile = MJCard::StringToCard(*(++it));
			for (int i = 0; i < 3; i++) {
				if (firstTile.value + i != centerTile.value) {
					MJCard tile = MJCard(firstTile.color, firstTile.value + i);
					player.Hand -= tile;
					player.discards += tile;
				}
			}
		}
		else if (command == COMMAND_PON) {
			MJCard tile = MJCard::StringToCard(tileStr);
			for (int i = 0; i < 2; i++) {
				player.Hand -= tile;
				player.discards += tile;
			}
		}
		else if (command == COMMAND_GON) {
			MJCard tile = MJCard::StringToCard(tileStr);
			for (int i = 0; i < 3; i++) {
				player.Hand -= tile;
				player.discards += tile;
			}
		}
		else if (command == COMMAND_PONGON) {
			MJCard tile = MJCard::StringToCard(tileStr);
			player.Hand -= tile;
			player.discards += tile;
		}
		else if (command == COMMAND_ANGON) {
			MJCard tile = MJCard::StringToCard(tileStr);
			for (int i = 0; i < 4; i++) {
				player.Hand -= tile;
			}
		}
		_lock.unlock();
	}));

	current_socket->on("BroadcastCommand", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		if (player.id != data[1]->get_int()) {
			int command = data[2]->get_int();
			string tileStr = data[3]->get_string();
			if (command == COMMAND_EAT) {
				boost::tokenizer<> tok(tileStr);
				boost::tokenizer<>::iterator it = tok.begin();
				MJCard firstTile = MJCard::StringToCard(*it);
				MJCard centerTile = MJCard::StringToCard(*(++it));
				for (int i = 0; i < 3; i++) {
					if (firstTile.value + i != centerTile.value) {
						player.discards += MJCard(firstTile.color, firstTile.value + i);
					}
				}
			}
			else if (command == COMMAND_PON) {
				MJCard tile = MJCard::StringToCard(tileStr);
				player.discards += tile;
				player.discards += tile;
			}
			else if (command == COMMAND_GON) {
				MJCard tile = MJCard::StringToCard(tileStr);
				player.discards += tile;
				player.discards += tile;
				player.discards += tile;
			}
			else if (command == COMMAND_PONGON) {
				MJCard tile = MJCard::StringToCard(tileStr);
				player.discards += tile;
			}
		}
		_lock.unlock();
	}));

	current_socket->on("end", sio::socket::event_listener_aux([&](string const& name, message::list const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		_lock.unlock();
	}));
}

MAIN_FUNC
{

    sio::client h;
    connection_listener l(h);

	// create new bot
	player = Irb01();
    
    h.set_open_listener(std::bind(&connection_listener::on_connected, &l));
    h.set_close_listener(std::bind(&connection_listener::on_close, &l,std::placeholders::_1));
    h.set_fail_listener(std::bind(&connection_listener::on_fail, &l));
    h.connect("http://140.118.127.157:3000");
    _lock.lock();
    if(!connect_finish)
    {
        _cond.wait(_lock);
    }
    _lock.unlock();
	current_socket = h.socket();
	bind_events();
	current_socket->emit("join", "bot-" + MathUtils::generateUUID(), [](message::list const& ack) {
		_lock.lock();
		uuid = ack[0]->get_string();
		_lock.unlock();
	});
	while(1) {
    }
    HIGHLIGHT("Closing...");
    h.sync_close();
    h.clear_con_listeners();
	return 0;
}

