/*************************************************************************
 * This file is part of winhide
 * github.con/univrsal/winhide
 * Copyright 2019 univrsal <universailp@web.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "server.hpp"
#include "util.hpp"

namespace network {

server::server(uint16_t port)
{
	m_ip.port = port;
	init();
}

server::~server()
{
	if (m_server_socket) {
		netlib_tcp_close(m_server_socket);
		info("Closing server socket at port %i", m_ip.port);
	}
	if (m_server_socket)
		netlib_free_byte_buf(m_buf);
	if (m_sockets)
		netlib_free_socket_set(m_sockets);
}

void server::init()
{
	if (netlib_resolve_host(&m_ip, nullptr, m_ip.port) < 0) {
		warn("Couldn't resolve local ip. Can't start network: %s",
		     netlib_get_error());
	} else {
		info("Resolved local ip %d.%d.%d.%d",
		     ip_shift(m_ip.host));
		m_server_socket = netlib_tcp_open(&m_ip);
		m_buf = netlib_alloc_byte_buf(0xff);
		m_sockets = netlib_alloc_socket_set(1);
		if (!m_buf) {
			warn("Couldn't allocate byte buffer: %s", netlib_get_error());
		} else if (!m_server_socket) {
			warn("Couldn't open socket: %s", netlib_get_error());
		} else if (!m_sockets) {
			warn("Couldn't allocate socket set: %s", netlib_get_error());
		} else if (netlib_tcp_add_socket(m_sockets, m_server_socket) < 0) {
			warn("Couldn't allocate socket set: %s", netlib_get_error());
		} else {
			m_started = true;
		}
	}
}

bool server::connect_client()
{
	bool result = true;
	netlib_free_socket_set(m_sockets);
	m_sockets = netlib_alloc_socket_set(2);
	if (m_sockets) {
		if (netlib_tcp_add_socket(m_sockets, m_server_socket) < 0 ||
		    netlib_tcp_add_socket(m_sockets, m_client_socket) < 0) {
			result = false;
		} else {

		}
	} else {
		result = false;
	}
	return result;
}

void server::disconnect_client()
{
	netlib_tcp_close(m_client_socket);
	m_client_socket = nullptr;
	netlib_free_socket_set(m_sockets);
	m_sockets = netlib_alloc_socket_set(1);
	netlib_tcp_add_socket(m_sockets, m_server_socket);
}

void server::tick(std::vector<rect>& windows, std::mutex& m)
{
	/* Check if there's new messages in socket
	   This might need to be moved into a thread
	   if the video_tick call is too slow
	*/
	int ready = netlib_check_socket_set(m_sockets, 25);

	if (ready == -1) {
		m_started = false;
		warn("Couldn't check network sockets. Closing server.");
		return;
	}

	if (ready == 0)
		return; /* nothing todo */

	/* First check server socket */
	if (netlib_socket_ready(m_server_socket)) {
		if (m_client_socket) {
			warn("Recieved second client connection. Ignoring it...");
		} else { /* Client connected for the first time */
			m_client_socket = netlib_tcp_accept(m_server_socket);
			if (!m_client_socket) {
				warn("Error while opening client connection");
			} else if (!connect_client()) {
				warn("Adding connection to client failed");
				disconnect_client();
			}
		}
	}

	if (netlib_socket_ready(m_client_socket)) {
		m.lock();
		receive_windows(windows);
		m.unlock();
	}
}

inline int read_rect(rect *r, netlib_byte_buf *b)
{
	return netlib_read_uint16(b, &r->x) &&
	       netlib_read_uint16(b, &r->y) &&
	       netlib_read_uint16(b, &r->w) &&
	       netlib_read_uint16(b, &r->h);
}

void server::receive_windows(std::vector<rect> &r)
{
	m_buf->read_pos = 0;
	uint8_t window_count = 0;
	int read = netlib_tcp_recv_buf(m_client_socket, m_buf);
	if (read != m_buf->length) {
		warn("Received partial message from client. Disconnecting!");
		disconnect_client();
		return;
	}

	if (netlib_read_uint8(m_buf, &window_count) < 0) {
		warn("Error reading window list size.");
		return;
	}

	r.clear();
	for (int i = 0; i < window_count; i++) {
		rect tmp { 0, 0, 0, 0};
		if (read_rect(&tmp, m_buf) < 0) {
			warn("Error while reading window at position %i from client.");
			break;
		}
		r.emplace_back(tmp);
	}
}

}
