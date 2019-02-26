#include "../src/sio_client.h"
#include "./UUID_generator.h"
#include <functional>
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

void bind_events()
{
	current_socket->on("readyToStart", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		room = data->get_string();
		message::list list = message::list();
		list.push(uuid);
		list.push(room);
		current_socket->emit("auth",  list);
		current_socket->emit("ready", list, [](message::list const& ack) {
			_lock.lock();
			// player.id = ack[0]->get_int();
			_lock.unlock();
		});
		_lock.unlock();
	}));

	current_socket->on("broadcastOpenDoor", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		// player.opendoorIdx = data->get_int();
		_lock.unlock();
	}));

	current_socket->on("broadcastWindAndRound", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		// player.windIdx  = data->get_vector()[0]->get_int();
		// player.roundIdx = data->get_vector()[1]->get_int();
		_lock.unlock();
	}));

	current_socket->on("dealTile", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		// player.hand += 
		_lock.unlock();
	}));

	current_socket->on("BroadcastBuHua", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		// player.flower +=
		_lock.unlock();
	}));

	current_socket->on("BroadcastHua", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		string id     = data->get_vector()[0]->get_string();
		string flower = data->get_vector()[1]->get_string();
		// if (id == player.id) {
		//     player.hand   -= 
		//     player.flower +=
		// }
		_lock.unlock();
	}));
	
	current_socket->on("draw", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		// string tile  = data->get_string();
		// player.hand +=
		_lock.unlock();
	}));

	current_socket->on("throw", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		// current_socket->emit("throwTile", player.Throw());
		_lock.unlock();
	}));
	current_socket->on("command", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		// current_socket->emit("sendCommand", player.OnCommand(data->get_string()));
		_lock.unlock();
	}));
	current_socket->on("end", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list &ack_resp) {
		_lock.lock();
		_lock.unlock();
	}));
}

MAIN_FUNC
{

    sio::client h;
    connection_listener l(h);

	// create new bot
    
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

