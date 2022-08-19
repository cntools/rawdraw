//Copyright 2015-2021 <>< Charles Lohr Under the MIT/x11 License, NewBSD License or
// ColorChord License.  You Choose.  This file mostly based on `cnhttp` from cntools.

#ifdef CNFGHTTP

//Pull from a buffer
#ifndef CNFGHTTP_LIVE_FS
#define USE_RAM_MFS
#endif

//single_file_http.c base from https://github.com/cntools/httptest.
//scroll to bottom for implementation.

#ifndef CUSTOM_HTTPHEADER_CODE
#define CUSTOM_HTTPHEADER_CODE 	PushString("Access-Control-Allow-Origin: *\r\n");
#endif

/* public api for steve reid's public domain SHA-1 implementation */
/* this file is in the public domain */


#include <stdint.h>
#include <malloc.h>

typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    uint8_t  buffer[64];
} RD_SHA1_CTX;

#define RD_SHA1_DIGEST_SIZE 20

void static RD_SHA1_Init(RD_SHA1_CTX* context);
void static RD_SHA1_Update(RD_SHA1_CTX* context, const uint8_t* data, const unsigned long len);
void static RD_SHA1_Final(uint8_t digest[RD_SHA1_DIGEST_SIZE],RD_SHA1_CTX* context);  //WARNINGThe parameters are flipped here. (CNL)

//Not to be confused with MFS for the AVR.

#ifndef _MFS_H
#define _MFS_H


#ifndef USE_RAM_MFS
#include <stdio.h>
#endif

#define MFS_SECTOR	256
#define MFS_FILENAMELEN 32-8
#define MFS_FILE_COMPRESSED_MEMORY (-2)

//Format:
//  [FILE NAME (24)] [Start (4)] [Len (4)]
//  NOTE: Filename must be null-terminated within the 24.
struct MFSFileEntry
{
	char name[MFS_FILENAMELEN];
	uint32_t start;  //From beginning of mfs thing.
	uint32_t len;
};


struct MFSFileInfo
{
	uint32_t filelen;
#ifdef USE_RAM_MFS
	uint32_t offset;
#else
	FILE * file;
#endif
};



//Returns 0 on succses.
//Returns size of file if non-empty
//If positive, populates mfi.
//Returns -1 if can't find file or reached end of file list.
int8_t MFSOpenFile( const char * fname, struct MFSFileInfo * mfi );
int32_t MFSReadSector( uint8_t* data, struct MFSFileInfo * mfi ); //returns # of bytes left in file.
void MFSClose( struct MFSFileInfo * mfi );



#endif


#ifndef _CNHTTP_H
#define _CNHTTP_H

#include <stdint.h>

extern struct HTTPConnection * curhttp;
extern uint8_t * curdata;
extern uint16_t  curlen;
extern uint8_t   wsmask[4];
extern uint8_t   wsmaskplace;



uint8_t WSPOPMASK();
#define HTTPPOP (*curdata++)

//You must provide this.
void HTTPCustomStart( );
void HTTPCustomCallback( );  //called when we can send more data
void WebSocketData( int len );
void WebSocketTick( );
void WebSocketNew();
void HTTPHandleInternalCallback( );
uint8_t hex2byte( const char * c );
void NewWebSocket();
void et_espconn_disconnect( int socket );

//Internal Functions
void HTTPTick( uint8_t timedtick ); 
int URLDecode( char * decodeinto, int maxlen, const char * buf );
void WebSocketGotData( uint8_t c );
void WebSocketTickInternal();
void WebSocketSend( uint8_t * data, int size );

//Host-level functions
void my_base64_encode(const unsigned char *data, unsigned int input_length, uint8_t * encoded_data );
void Uint32To10Str( char * out, uint32_t dat );
void http_recvcb(int conn, char *pusrdata, unsigned short length);
void http_disconnetcb(int conn);
int httpserver_connectcb( int socket ); // return which HTTP it is.  -1 for failure
void DataStartPacket();
extern uint8_t * databuff_ptr;
void PushString( const char * data );
void PushByte( uint8_t c );
void PushBlob( const uint8_t * datam, int len );
int TCPCanSend( int socket, int size );
int TCPDoneSend( int socket );
int EndTCPWrite( int socket );

#define HTTP_CONNECTIONS 50
#ifndef MAX_HTTP_PATHLEN
#define MAX_HTTP_PATHLEN 80
#endif
#define HTTP_SERVER_TIMEOUT		500


#define HTTP_STATE_NONE        0
#define HTTP_STATE_WAIT_METHOD 1
#define HTTP_STATE_WAIT_PATH   2
#define HTTP_STATE_WAIT_PROTO  3

#define HTTP_STATE_WAIT_FLAG   4
#define HTTP_STATE_WAIT_INFLAG 5
#define HTTP_STATE_DATA_XFER   7
#define HTTP_STATE_DATA_WEBSOCKET   8

#define HTTP_WAIT_CLOSE        15

struct HTTPConnection
{
	uint8_t  state:4;
	uint8_t  state_deets;

	//Provides path, i.e. "/index.html" but, for websockets, the last 
	//32 bytes of the buffer are used for the websockets key.  
	uint8_t  pathbuffer[MAX_HTTP_PATHLEN];
	uint8_t  is_dynamic:1;
	uint16_t timeout;

	union data_t
	{
		struct MFSFileInfo filedescriptor;
		struct UserData { uint16_t a, b, c; } user;
		struct UserDataPtr { void * v; } userptr;
	} data;

	void * rcb;
	void * rcbDat; //For websockets primarily.
	void * ccb;                          //Close callback (used for websockets, primarily)

	uint32_t bytesleft;
	uint32_t bytessofar;

	uint8_t  is404:1;
	uint8_t  isdone:1;
	uint8_t  isfirst:1;
	uint8_t  keep_alive:1;
	uint8_t  need_resend:1;
	uint8_t  send_pending:1; //If we can send data, we should?
	uint8_t  is_gzip:1;

	int socket;
	uint8_t corked_data[4096];
	int corked_data_place;
};

extern struct HTTPConnection HTTPConnections[HTTP_CONNECTIONS];

#endif

#ifndef _HTTP_BSD_H
#define _HTTP_BSD_H


//Call this to start your webserver.
int RunHTTP( int port );
int TickHTTP(); //returns -1 if problem.

//For running on a BSD Sockets System
int htsend( int socket, uint8_t * data, int datact );
void et_espconn_disconnect( int socket );
void http_recvcb(int whichhttp, char *pusrdata, unsigned short length);
void http_disconnetcb(int whichhttp);
int httpserver_connectcb( int socket ); // return which HTTP it is.  -1 for failure
void DataStartPacket();
extern uint8_t * databuff_ptr;
void PushBlob( const uint8_t * data, int len );
void PushByte( uint8_t c );
void PushString( const char * data );
int TCPCanSend( int socket, int size );
int TCPDoneSend( int socket );
int EndTCPWrite( int socket );
void TermHTTPServer();

extern int cork_binary_rx;

#endif

//Copyright 2012-2016 <>< Charles Lohr Under the MIT/x11 License, NewBSD License or ColorChord License.  You Choose.

#include <string.h>
#include <stdio.h>

struct HTTPConnection HTTPConnections[HTTP_CONNECTIONS];

#define HTDEBUG( x, ... ) printf( x, ##__VA_ARGS__ )
//#define HTDEBUG( x... )

//#define ISKEEPALIVE "keep-alive"
#define ISKEEPALIVE "close"

struct HTTPConnection HTTPConnections[HTTP_CONNECTIONS];
struct HTTPConnection * curhttp;
uint8_t * curdata;
uint16_t  curlen;
uint8_t   wsmask[4];
uint8_t   wsmaskplace;


void CloseEvent();
void InternalStartHTTP( );
void HTTPHandleInternalCallback( );

void HTTPClose( )
{
	//This is dead code, but it is a testament to Charles.
	//Do not do this here.  Wait for the ESP to tell us the
	//socket is successfully closed.
	//curhttp->state = HTTP_STATE_NONE;
	curhttp->state = HTTP_WAIT_CLOSE;
	et_espconn_disconnect( curhttp->socket ); 
	CloseEvent();
}


void  HTTPGotData( )
{
	uint8_t c;
	curhttp->timeout = 0;
	while( curlen-- )
	{
		c = HTTPPOP;
	//	sendhex2( h->state ); sendchr( ' ' );

		switch( curhttp->state )
		{
		case HTTP_STATE_WAIT_METHOD:
			if( c == ' ' )
			{
				curhttp->state = HTTP_STATE_WAIT_PATH;
				curhttp->state_deets = 0;
			}
			break;
		case HTTP_STATE_WAIT_PATH:
			curhttp->pathbuffer[curhttp->state_deets++] = c;
			if( curhttp->state_deets == MAX_HTTP_PATHLEN )
			{
				curhttp->state_deets--;
			}
			
			if( c == ' ' )
			{
				//Tricky: If we're a websocket, we need the whole header.
				curhttp->pathbuffer[curhttp->state_deets-1] = 0;
				curhttp->state_deets = 0;

				if( strncmp( (const char*)curhttp->pathbuffer, "/d/ws", 5 ) == 0 )
				{
					curhttp->state = HTTP_STATE_DATA_WEBSOCKET;
					curhttp->state_deets = 0;
				}
				else
				{
					curhttp->state = HTTP_STATE_WAIT_PROTO; 
				}
			}
			break;
		case HTTP_STATE_WAIT_PROTO:
			if( c == '\n' )
			{
				curhttp->state = HTTP_STATE_WAIT_FLAG;
			}
			break;
		case HTTP_STATE_WAIT_FLAG:
			if( c == '\n' )
			{
				curhttp->state = HTTP_STATE_DATA_XFER;
				InternalStartHTTP( );
			}
			else if( c != '\r' )
			{
				curhttp->state = HTTP_STATE_WAIT_INFLAG;
			}
			break;
		case HTTP_STATE_WAIT_INFLAG:
			if( c == '\n' )
			{
				curhttp->state = HTTP_STATE_WAIT_FLAG;
				curhttp->state_deets = 0;
			}
			break;
		case HTTP_STATE_DATA_XFER:
			//Ignore any further data?
			curlen = 0;
			break;
		case HTTP_STATE_DATA_WEBSOCKET:
			WebSocketGotData( c );
			break;
		case HTTP_WAIT_CLOSE:
			if( curhttp->keep_alive )
			{
				curhttp->state = HTTP_STATE_WAIT_METHOD;
			}
			else
			{
				HTTPClose( );
			}
			break;
		default:
			break;
		};
	}

}


static void DoHTTP( uint8_t timed )
{
	switch( curhttp->state )
	{
	case HTTP_STATE_NONE: //do nothing if no state.
		curhttp->send_pending = 0;
		break;
	case HTTP_STATE_DATA_XFER:
		curhttp->send_pending = 1;
		if( TCPCanSend( curhttp->socket, 1300 ) ) //TCPDoneSend
		{
			if( curhttp->is_dynamic )
			{
				HTTPCustomCallback( );
			}
			else
			{
				HTTPHandleInternalCallback( );
			}
		}
		break;
	case HTTP_WAIT_CLOSE:
		curhttp->send_pending = 0;
		if( TCPDoneSend( curhttp->socket ) )
		{
			if( curhttp->keep_alive )
			{
				curhttp->state = HTTP_STATE_WAIT_METHOD;
			}
			else
			{
				HTTPClose( );
			}
		}
		break;
	case HTTP_STATE_DATA_WEBSOCKET:
		curhttp->send_pending = 0;
		if( TCPCanSend( curhttp->socket, 1300 ) ) //TCPDoneSend
		{
			WebSocketTickInternal();
		}
		break;
	default:
		if( timed )
		{
			if( curhttp->timeout++ > HTTP_SERVER_TIMEOUT )
			{
				HTTPClose( );
			}
		}
	}
}

void  HTTPTick( uint8_t timed )
{
	uint8_t i;
	for( i = 0; i < HTTP_CONNECTIONS; i++ )
	{
		if( curhttp ) { HTDEBUG( "HTTPRXQ\n" ); break; }
		curhttp = &HTTPConnections[i];
		DoHTTP( timed );
		curhttp = 0;
	}
}

void  HTTPHandleInternalCallback( )
{
	uint16_t i, bytestoread;

	if( curhttp->isdone )
	{
		HTTPClose( );
		return;
	}
	if( curhttp->is404 )
	{
		DataStartPacket();
		PushString("HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\nFile not found.");
		EndTCPWrite( curhttp->socket );
		curhttp->isdone = 1;
		return;
	}
	if( curhttp->isfirst )
	{
		char stto[10];
		uint8_t slen = strlen( curhttp->pathbuffer );
		const char * k;

		DataStartPacket();;
		//TODO: Content Length?  MIME-Type?
		PushString("HTTP/1.1 200 Ok\r\n");

#ifdef CUSTOM_HTTPHEADER_CODE
		CUSTOM_HTTPHEADER_CODE
#endif

		if( curhttp->bytesleft < 0xfffffffe )
		{
			PushString("Connection: "ISKEEPALIVE"\r\nContent-Length: ");
			Uint32To10Str( stto, curhttp->bytesleft );
			PushBlob( stto, strlen( stto ) );
			curhttp->keep_alive = 1;
		}
		else
		{
			PushString("Connection: close");
			curhttp->keep_alive = 0;
		}

		PushString( "\r\nContent-Type: " );
		//Content-Type?
		while( slen && ( curhttp->pathbuffer[--slen] != '.' ) );
		k = &curhttp->pathbuffer[slen+1];

		if( strcmp( k, "mp3" ) == 0 )      PushString( "audio/mpeg3" );
		else if( strcmp( k, "jpg" ) == 0 ) PushString( "image/jpeg" );
		else if( strcmp( k, "png" ) == 0 ) PushString( "image/png" );
		else if( strcmp( k, "css" ) == 0 ) PushString( "text/css" );
		else if( strcmp( k, "js" ) == 0 )  PushString( "text/javascript" );
		else if( strcmp( k, "gz" ) == 0 )  PushString( "text/plain\r\nContent-Encoding: gzip\r\nCache-Control: public, max-age=3600" );			
		else if( curhttp->bytesleft == 0xfffffffe ) PushString( "text/plain" );
		else                               PushString( "text/html" );

		if( curhttp->is_gzip ) PushString( "\r\nContent-Encoding: gzip" ); 

		PushString( "\r\n\r\n" );
		EndTCPWrite( curhttp->socket );
		curhttp->isfirst = 0;

		return;
	}

	DataStartPacket();

	for( i = 0; i < 2 && curhttp->bytesleft; i++ )
	{
		int bpt = curhttp->bytesleft;
		if( bpt > MFS_SECTOR ) bpt = MFS_SECTOR;
		curhttp->bytesleft = MFSReadSector( databuff_ptr, &curhttp->data.filedescriptor );
		databuff_ptr += bpt;
	}

	EndTCPWrite( curhttp->socket );

	if( !curhttp->bytesleft )
		curhttp->isdone = 1;
}

void InternalStartHTTP( )
{
	int32_t clusterno;
	int8_t i;
	char * path = &curhttp->pathbuffer[0];

	curhttp->is_gzip = 0;

	if( curhttp->pathbuffer[0] == '/' )
		path++;

	if( path[0] == 'd' && path[1] == '/' )
	{
		curhttp->is_dynamic = 1;
		curhttp->isfirst = 1;
		curhttp->isdone = 0;
		curhttp->is404 = 0;
		HTTPCustomStart();
		return;
	}

	if( !path[0] )
	{
		path = "index.html";
	}

	for( i = 0; path[i]; i++ )
		if( path[i] == '?' ) path[i] = 0;

	i = MFSOpenFile( path, &curhttp->data.filedescriptor );
	
	curhttp->bytessofar = 0;
	curhttp->is_gzip = 0;

	if( i == MFS_FILE_COMPRESSED_MEMORY )
	{
		curhttp->is_gzip = 1;
	}
	else if( i < 0 )
	{
		HTDEBUG( "404(%s)\n", path );
		curhttp->is404 = 1;
		curhttp->isfirst = 1;
		curhttp->isdone = 0;
		curhttp->is_dynamic = 0;
		curhttp->bytesleft = 0;
		return;
	}

	curhttp->isfirst = 1;
	curhttp->isdone = 0;
	curhttp->is404 = 0;
	curhttp->is_dynamic = 0;
	curhttp->bytesleft = curhttp->data.filedescriptor.filelen;
}


void http_disconnetcb(int conn ) {
	int r = conn;
	if( r>=0 )
	{
		if( !HTTPConnections[r].is_dynamic ) MFSClose( &HTTPConnections[r].data.filedescriptor );
		HTTPConnections[r].state = 0;
	}
}


void http_recvcb(int conn, char *pusrdata, unsigned short length)
{
	int whichhttp = conn;
	//Though it might be possible for this to interrupt the other
	//tick task, I don't know if this is actually a probelem.
	//I'm adding this back-up-the-register just in case.
	if( curhttp ) { HTDEBUG( "Unexpected Race Condition\n" ); return; }

	curhttp = &HTTPConnections[whichhttp];
	curdata = (uint8_t*)pusrdata;
	curlen = length;
	HTTPGotData();
	curhttp = 0 ;

}

int httpserver_connectcb( int socket )
{
	int i;

	for( i = 0; i < HTTP_CONNECTIONS; i++ )
	{
		if( HTTPConnections[i].state == 0 )
		{
			HTTPConnections[i].socket = socket;
			HTTPConnections[i].state = HTTP_STATE_WAIT_METHOD;
			break;
		}
	}
	if( i == HTTP_CONNECTIONS )
	{
		return -1;
	}

	return i;
}


int  URLDecode( char * decodeinto, int maxlen, const char * buf )
{
	int i = 0;

	for( ; buf && *buf; buf++ )
	{
		char c = *buf;
		if( c == '+' )
		{
			decodeinto[i++] = ' ';
		}
		else if( c == '?' || c == '&' )
		{
			break;
		}
		else if( c == '%' )
		{
			if( *(buf+1) && *(buf+2) )
			{
				decodeinto[i++] = hex2byte( buf+1 );
				buf += 2;
			}
		}
		else
		{
			decodeinto[i++] = c;
		}
		if( i >= maxlen -1 )  break;
		
	}
	decodeinto[i] = 0;
	return i;
}


#ifndef RD_SHA1_HASH_LEN
#define RD_SHA1_HASH_LEN RD_SHA1_DIGEST_SIZE
#endif

void  WebSocketGotData( uint8_t c )
{
	switch( curhttp->state_deets )
	{
	case 0:
	{
		int i = 0;
		char inkey[120];
		unsigned char hash[RD_SHA1_HASH_LEN];
		RD_SHA1_CTX c;
		int inkeylen = 0;

		curhttp->is_dynamic = 1;
		while( curlen > 20 )
		{
			curdata++; curlen--;
			if( strncmp( curdata, "Sec-WebSocket-Key: ", 19 ) == 0 )
			{
				break;
			}
		}

		if( curlen <= 21 )
		{
			HTDEBUG( "No websocket key found.\n" );
			curhttp->state = HTTP_WAIT_CLOSE;
			return;
		}

		curdata+= 19;
		curlen -= 19;


#define WS_KEY_LEN 36
#define WS_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define WS_RETKEY_SIZEM1 32

		while( curlen > 1 )
		{
			uint8_t lc = *(curdata++);
			inkey[i] = lc;
			curlen--;
			if( lc == '\r' )
			{
				inkey[i] = 0;
				break;
			}
			i++;
			if( i >= sizeof( inkey ) - WS_KEY_LEN - 5 )
			{
				HTDEBUG( "Websocket key too big.\n" );
				curhttp->state = HTTP_WAIT_CLOSE;
				return;
			}
		}
		if( curlen <= 1 )
		{
			HTDEBUG( "Invalid websocket key found.\n" );
			curhttp->state = HTTP_WAIT_CLOSE;
			return;
		}

		if( i + WS_KEY_LEN + 1 >= sizeof( inkey ) )
		{
			HTDEBUG( "WSKEY Too Big.\n" );
			curhttp->state = HTTP_WAIT_CLOSE;
			return;
		}

		memcpy( &inkey[i], WS_KEY, WS_KEY_LEN + 1 );
		i += WS_KEY_LEN;
		RD_SHA1_Init( &c );
		RD_SHA1_Update( &c, inkey, i );
		RD_SHA1_Final( hash, &c );

#if	(WS_RETKEY_SIZE > MAX_HTTP_PATHLEN - 10 )
#error MAX_HTTP_PATHLEN too short.
#endif

		my_base64_encode( hash, RD_SHA1_HASH_LEN, curhttp->pathbuffer + (MAX_HTTP_PATHLEN-WS_RETKEY_SIZEM1)  );

		curhttp->bytessofar = 0;
		curhttp->bytesleft = 0;

		NewWebSocket();
		//Respond...
		curhttp->state_deets = 1;
		break;
	}
	case 1:
		if( c == '\n' ) curhttp->state_deets = 2;
		break;
	case 2:
		if( c == '\r' ) curhttp->state_deets = 3;
		else curhttp->state_deets = 1;
		break;
	case 3:
		if( c == '\n' ) curhttp->state_deets = 4;
		else curhttp->state_deets = 1;
		break;
	case 5: //Established connection.
	{
		//XXX TODO: Seems to malfunction on large-ish packets.  I know it has problems with 140-byte payloads.

		do
		{
			if( curlen < 5 ) //Can't interpret packet.
				break;

			uint8_t fin = c & 1;
			uint8_t opcode = c << 4;
			uint16_t payloadlen = *(curdata++);

			curlen--;
			if( !(payloadlen & 0x80) )
			{
				HTDEBUG( "Unmasked packet (%d)\n", payloadlen );
				curhttp->state = HTTP_WAIT_CLOSE;
				break;
			}

			if( opcode == 128 )
			{
				//Close connection.
				//HTDEBUG( "CLOSE\n" );
				//curhttp->state = HTTP_WAIT_CLOSE;
				//break;
			}

			payloadlen &= 0x7f;
			if( payloadlen == 127 )
			{
				//Very long payload.
				//Not supported.
				HTDEBUG( "Unsupported payload packet.\n" );
				curhttp->state = HTTP_WAIT_CLOSE;
				break;
			}
			else if( payloadlen == 126 )
			{
				payloadlen = (curdata[0] << 8) | curdata[1];
				curdata += 2;
				curlen -= 2;
			}
			wsmask[0] = curdata[0];
			wsmask[1] = curdata[1];
			wsmask[2] = curdata[2];
			wsmask[3] = curdata[3];
			curdata += 4;
			curlen -= 4;
			wsmaskplace = 0;

			//XXX Warning: When packets get larger, they may split the
			//websockets packets into multiple parts.  We could handle this
			//but at the cost of prescious RAM.  I am chosing to just drop those
			//packets on the floor, and restarting the connection.
			if( curlen < payloadlen )
			{
				extern int cork_binary_rx;
				cork_binary_rx = 1;
				//HTDEBUG( "Websocket Fragmented. %d %d\n", curlen, payloadlen );
				//curhttp->state = HTTP_WAIT_CLOSE;
				HTDEBUG( "Websocket Fragmented. %d %d\n", curlen, payloadlen );
				curhttp->state = HTTP_WAIT_CLOSE;
				return;
			}

			char * newcurdata = curdata + payloadlen + 1;
			WebSocketData( payloadlen );
			curlen -= payloadlen; 
			curdata = newcurdata;
		}
		while( curlen > 5 );
		break;
	}
	default:
		break;
	}
}

void  WebSocketTickInternal()
{
	switch( curhttp->state_deets )
	{
	case 4: //Has key full HTTP header, etc. wants response.
		DataStartPacket();;
		PushString( "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: " );
		PushString( curhttp->pathbuffer + (MAX_HTTP_PATHLEN-WS_RETKEY_SIZEM1) );
		PushString( "\r\n\r\n" );
		EndTCPWrite( curhttp->socket );
		curhttp->state_deets = 5;
		curhttp->keep_alive = 0;
		break;
	case 5:
		WebSocketTick();
		break;
	}
}

void  WebSocketSend( uint8_t * data, int size )
{
	DataStartPacket();;
	PushByte( 0x82 ); //0x81 is text.
	if( size >= 126 )
	{
		PushByte( 0x00 | 126 );
		PushByte( size>>8 );
		PushByte( size&0xff );
	}
	else
	{
		PushByte( 0x00 | size );
	}
	PushBlob( data, size );
	EndTCPWrite( curhttp->socket );
	curhttp->send_pending = 1;
}

uint8_t WSPOPMASK()
{
	uint8_t mask = wsmask[wsmaskplace];
	wsmaskplace = (wsmaskplace+1)&3;
	return (*curdata++)^(mask);
}






#if defined(WINDOWS) || defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#define socklen_t uint32_t
#define SHUT_RDWR SD_BOTH
#define MSG_NOSIGNAL 0
#else
#define closesocket close
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <unistd.h>
#include <sys/time.h>
uint16_t htons(uint16_t hostshort);
#endif

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static  int serverSocket;

uint8_t * databuff_ptr;
uint8_t   databuff[1536];
int cork_binary_rx;


#ifndef HTTP_POLL_TIMEOUT
#define HTTP_POLL_TIMEOUT 0 //This is a little weird, it's an adaptation from cnhttp.
#endif


#define DESTROY_SOCKETS_LIST 200
int destroy_sockets[DESTROY_SOCKETS_LIST];
int destroy_socket_head = 0;
int sockets[HTTP_CONNECTIONS];

void et_espconn_disconnect( int socket )
{
	shutdown( socket, SHUT_RDWR );
	int i;
	//printf( "Shut: %d\n", socket );
	for( i = 0; i < HTTP_CONNECTIONS; i++ )
	{
		if( sockets[i] == socket )
		{
			http_disconnetcb( i );
			sockets[i] = 0;
		}
	}

	if( destroy_sockets[destroy_socket_head] ) closesocket( destroy_sockets[destroy_socket_head] );
	destroy_sockets[destroy_socket_head] = socket;
	destroy_socket_head = (destroy_socket_head+1)%DESTROY_SOCKETS_LIST;

}

void DataStartPacket()
{
	databuff_ptr = databuff;
}

void PushByte( uint8_t c )
{
	if( databuff_ptr - databuff + 1 >= sizeof( databuff ) ) return;
	*(databuff_ptr++) = c;
}

void PushBlob( const uint8_t * data, int len )
{
	if( databuff_ptr - databuff + len >= sizeof( databuff ) ) return;
	memcpy( databuff_ptr, data, len );
	databuff_ptr += len;
}

void PushString( const char * data )
{
	int len = strlen( data );
	if( databuff_ptr - databuff + len >= sizeof( databuff ) ) return;
	memcpy( databuff_ptr, data, len );
	databuff_ptr += len;
}

int TCPCanSend( int socket, int size )
{
	fd_set write_fd_set;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO (&write_fd_set);
	FD_SET (socket, &write_fd_set);

	int r = select (FD_SETSIZE, NULL, &write_fd_set, NULL, &tv);
	if (r < 0)
    {
		perror ("select");
		return -1;
    }
	return r;
}


int TCPCanRead( int sock )
{
	fd_set read_fd_set;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO (&read_fd_set);
	FD_SET (sock, &read_fd_set);

	int r;
	r = select (FD_SETSIZE, &read_fd_set, NULL, NULL, &tv);
	if (r < 0)
    {
		perror ("select");
		return -1;
    }
	return r;
}


int TCPException( int sock )
{
	int error_code;
	int error_code_size = sizeof(error_code);
	getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&error_code, &error_code_size);
	if( error_code >= 0 ) return 0;
	else return 1;
}

int TCPDoneSend( int socket )
{
	return TCPCanSend( socket, 1 );
}


int EndTCPWrite( int socket )
{
	int r = send( socket, databuff, databuff_ptr-databuff, MSG_NOSIGNAL );
	databuff_ptr = databuff;
	return r;
}

void TermHTTPServer()
{
	shutdown( serverSocket, SHUT_RDWR );
}

int TickHTTP()
{
	int i;
	//struct pollfd allpolls[HTTP_CONNECTIONS+1];
	short mappedhttp[HTTP_CONNECTIONS+1];
	if( serverSocket == 0 ) return -1;

	do
	{
		static double last;
		double now;
#if defined(WINDOWS) || defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
		static LARGE_INTEGER lpf;
		LARGE_INTEGER li;

		if( !lpf.QuadPart )
		{
			QueryPerformanceFrequency( &lpf );
		}

		QueryPerformanceCounter( &li );
		now = (double)li.QuadPart / (double)lpf.QuadPart;
#else
		struct timeval tv;
		gettimeofday( &tv, 0 );
		now = ((double)tv.tv_usec)/1000000. + (tv.tv_sec);
#endif
		double dl = now - last;
		if( dl > .1 )
		{
			int i;
			HTTPTick( 1 );
			last = now;
		}
		else
		{
			HTTPTick( 0 );
		}

/*		int pollct = 1;
		allpolls[0].fd = serverSocket;
		allpolls[0].events = LISTENPOLL;
		for( i = 0; i < HTTP_CONNECTIONS;i ++)
		{
			if( !sockets[i] || HTTPConnections[i].state == 0 ) continue;
			allpolls[pollct].fd = sockets[i];
			allpolls[pollct].events = POLLIN | (HTTPConnections[i].send_pending?POLLOUT:0);
			mappedhttp[pollct] = i;
			pollct++;
		}

		//Do something to watch all currently-waiting sockets.
		poll( allpolls, pollct, HTTP_POLL_TIMEOUT ); 
*/
		//If there's faults, bail.
		if( TCPException( serverSocket ) )
		{
			closesocket( serverSocket );
			for( i = 0; i < HTTP_CONNECTIONS;i ++)
			{
				if( sockets[i] ) closesocket( sockets[i] );
			}
			break;
		}
		if( TCPCanRead( serverSocket ) )
		{
			struct   sockaddr_in tin;
			socklen_t addrlen  = sizeof(tin);
			memset( &tin, 0, addrlen );
			int tsocket = accept( serverSocket, (struct sockaddr *)&tin, &addrlen );

#if defined(WINDOWS) || defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
			struct linger lx;
			lx.l_onoff = 1;
			lx.l_linger = 0;

			//Disable the linger here, too.
			setsockopt( tsocket, SOL_SOCKET, SO_LINGER, (const char*)&lx, sizeof( lx ) );
#else
			struct linger lx;
			lx.l_onoff = 1;
			lx.l_linger = 0;

			//Disable the linger here, too.
			setsockopt( tsocket, SOL_SOCKET, SO_LINGER, &lx, sizeof( lx ) );
#endif

			int r = httpserver_connectcb( tsocket );

			if( r == -1 )
			{
				closesocket( tsocket );
			}
			else
			{
				sockets[r] = tsocket;
			}
		}
		for( i = 0; i < HTTP_CONNECTIONS; i++)
		{
			int wc = i;
			if( !sockets[i] || HTTPConnections[i].state == 0 ) continue;
			if( TCPException(sockets[i]) )
			{
				http_disconnetcb( wc );
				closesocket( sockets[wc] );
				sockets[wc] = 0;
			}
			else if( TCPCanRead( sockets[i] ) )
			{
				int dco = HTTPConnections[i].corked_data_place;
				uint8_t data[8192];
				memcpy( data, HTTPConnections[i].corked_data, dco );
				int len = recv( sockets[wc], data+dco, 8192-dco, 0 );
				if( len )
				{
					cork_binary_rx = 0;
					http_recvcb( wc, data, len+dco );
					if( cork_binary_rx )
					{
						int to_cork = len;
						if( to_cork > sizeof( HTTPConnections[i].corked_data ) + HTTPConnections[i].corked_data_place )
						{
							http_disconnetcb( wc );
							closesocket ( sockets[wc] );
							sockets[wc] = 0;
							fprintf( stderr, "Error: too much data to buffer on websocket\n" );
						}
						else
						{
							memcpy( HTTPConnections[i].corked_data + dco, data + dco, to_cork );
							HTTPConnections[i].corked_data_place += to_cork;
						}
					}
					else
					{
						HTTPConnections[i].corked_data_place = 0;
					}
				}
				else
				{
					http_disconnetcb( wc );
					closesocket( sockets[wc] );
					sockets[wc] = 0;
				}
			}
		}
	}
#if HTTP_POLL_TIMEOUT != 0
	while(1);
#else
	while(0);
#endif

	return 0;
}


int RunHTTP( int port )
{
#if defined(WINDOWS) || defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        fprintf( stderr, "WSAStartup failed with error: %d\n", err);
        return 1;
    }
}
#endif


	int acceptfaults = 0;
	struct	sockaddr_in sin;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	//Make sure the socket worked.
	if( serverSocket == -1 )
	{
		fprintf( stderr, "Error: Cannot create socket.\n" );
		return -1;
	}

	//Disable SO_LINGER (Well, enable it but turn it way down)
#if defined(WINDOWS) || defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
	struct linger lx;
	lx.l_onoff = 1;
	lx.l_linger = 0;
	setsockopt( serverSocket, SOL_SOCKET, SO_LINGER, (const char *)&lx, sizeof( lx ) );

	//Enable SO_REUSEADDR
	int reusevar = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reusevar, sizeof(reusevar));
#else
	struct linger lx;
	lx.l_onoff = 1;
	lx.l_linger = 0;
	setsockopt( serverSocket, SOL_SOCKET, SO_LINGER, &lx, sizeof( lx ) );

	//Enable SO_REUSEADDR
	int reusevar = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reusevar, sizeof(reusevar));
#endif
	//Setup socket for listening address.
	memset( &sin, 0, sizeof( sin ) );
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons( port );

	//Actually bind to the socket
	if( bind( serverSocket, (struct sockaddr *) &sin, sizeof( sin ) ) == -1 )
	{
		fprintf( stderr, "Could not bind to socket: %d\n", port );
		closesocket( serverSocket );
		serverSocket = 0;
		return -1;
	}

	//Finally listen.
	if( listen( serverSocket, 5 ) == -1 )
	{
		fprintf(stderr, "Could not lieten to socket.");
		closesocket( serverSocket );
		serverSocket = 0;
		return -1;
	}

	return 0;
}






void Uint32To10Str( char * out, uint32_t dat )
{
	int tens = 1000000000;
	int val;
	int place = 0;

	while( tens > 1 )
	{
		if( dat/tens ) break;
		tens/=10;
	}

	while( tens )
	{
		val = dat/tens;
		dat -= val*tens;
		tens /= 10;
		out[place++] = val + '0';
	}

	out[place] = 0;
}

//from http://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
static const char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                      'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                      'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                      'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                      'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                      'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                      'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                      '4', '5', '6', '7', '8', '9', '+', '/'};

static const int mod_table[] = {0, 2, 1};

void my_base64_encode(const unsigned char *data, unsigned int input_length, uint8_t * encoded_data )
{

	int i, j;
    int output_length = 4 * ((input_length + 2) / 3);

    if( !encoded_data ) return;
	if( !data ) { encoded_data[0] = '='; encoded_data[1] = 0; return; }

    for (i = 0, j = 0; i < input_length; ) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[output_length - 1 - i] = '=';

	encoded_data[j] = 0;
}

uint8_t hex1byte( char c )
{
	if( c >= '0' && c <= '9' ) return c - '0';
	if( c >= 'a' && c <= 'f' ) return c - 'a' + 10;
	if( c >= 'A' && c <= 'F' ) return c - 'A' + 10;
	return 0;
}

uint8_t hex2byte( const char * c )
{
	return (hex1byte(c[0])<<4) | (hex1byte(c[1]));
}

#include <string.h>

#ifdef USE_RAM_MFS

#include "tools/rawdraw_http_page.h"

//Returns 0 on succses.
//Returns size of file if non-empty
//If positive, populates mfi.
//Returns -1 if can't find file or reached end of file list.
int8_t MFSOpenFile( const char * fname, struct MFSFileInfo * mfi )
{
#ifdef CNFG_DISABLE_HTTP_FILES
	mfi->filelen = 0;
	return -1;
#else
	if( strcmp( fname, "/" ) == 0 || strcmp( fname, "index.html" ) == 0 )
	{
		mfi->offset = 0;
		mfi->filelen = sizeof(webpage_buffer);
		return MFS_FILE_COMPRESSED_MEMORY;
	}
	else
		return -1;
#endif
}

int32_t MFSReadSector( uint8_t* data, struct MFSFileInfo * mfi )
{
#ifdef CNFG_DISABLE_HTTP_FILES
	return 0;
#else
	 //returns # of bytes left tin file.
	if( !mfi->filelen )
	{
		return 0;
	}

	int toread = mfi->filelen;
	if( toread > MFS_SECTOR ) toread = MFS_SECTOR;
	memcpy( data, &webpage_buffer[mfi->offset], toread );
	mfi->offset += toread;
	mfi->filelen -= toread;
	return mfi->filelen;
#endif
}

void MFSClose( struct MFSFileInfo * mfi )
{
}

#else


//Returns 0 on succses.
//Returns size of file if non-empty
//If positive, populates mfi.
//Returns -1 if can't find file or reached end of file list.
int8_t MFSOpenFile( const char * fname, struct MFSFileInfo * mfi )
{
	char targfile[1024];

	if( strlen( fname ) == 0 || fname[strlen(fname)-1] == '/' )
	{
		snprintf( targfile, sizeof( targfile ) - 1, "tools/rawdraw_http_files/%s/index.html", fname );
	}
	else
	{
		snprintf( targfile, sizeof( targfile ) - 1, "tools/rawdraw_http_files/%s", fname );
	}

	//printf( ":%s:\n", targfile );

	FILE * f = mfi->file = fopen( targfile, "rb" );
	if( f <= 0 ) return -1;
	//printf( "F: %p\n", f );
	fseek( f, 0, SEEK_END );
	mfi->filelen = ftell( f );
	fseek( f, 0, SEEK_SET );
	return 0;
}

int32_t MFSReadSector( uint8_t* data, struct MFSFileInfo * mfi )
{
	if( !mfi->filelen )
	{
		return 0;
	}

	int toread = fread( data, 1, MFS_SECTOR, mfi->file );
	mfi->filelen -= toread;
	return mfi->filelen;
}

void MFSClose( struct MFSFileInfo * mfi )
{
	if( mfi->file ) fclose( mfi->file );
}


#endif


/*
SHA-1 in C
By Steve Reid <sreid@sea-to-sky.net>
100% Public Domain

-----------------
Modified 7/98
By James H. Brown <jbrown@burgoyne.com>
Still 100% Public Domain

Corrected a problem which generated improper hash values on 16 bit machines
Routine SHA1Update changed from
	void SHA1Update(SHA1_CTX* context, unsigned char* data, unsigned int
len)
to
	void SHA1Update(SHA1_CTX* context, unsigned char* data, unsigned
long len)

The 'len' parameter was declared an int which works fine on 32 bit machines.
However, on 16 bit machines an int is too small for the shifts being done
against
it.  This caused the hash function to generate incorrect values if len was
greater than 8191 (8K - 1) due to the 'len << 3' on line 3 of SHA1Update().

Since the file IO in main() reads 16K at a time, any file 8K or larger would
be guaranteed to generate the wrong hash (e.g. Test Vector #3, a million
"a"s).

I also changed the declaration of variables i & j in SHA1Update to
unsigned long from unsigned int for the same reason.

These changes should make no difference to any 32 bit implementations since
an
int and a long are the same size in those environments.

--
I also corrected a few compiler warnings generated by Borland C.
1. Added #include <process.h> for exit() prototype
2. Removed unused variable 'j' in SHA1Final
3. Changed exit(0) to return(0) at end of main.

ALL changes I made can be located by searching for comments containing 'JHB'
-----------------
Modified 8/98
By Steve Reid <sreid@sea-to-sky.net>
Still 100% public domain

1- Removed #include <process.h> and used return() instead of exit()
2- Fixed overwriting of finalcount in SHA1Final() (discovered by Chris Hall)
3- Changed email address from steve@edmweb.com to sreid@sea-to-sky.net

-----------------
Modified 4/01
By Saul Kravitz <Saul.Kravitz@celera.com>
Still 100% PD
Modified to run on Compaq Alpha hardware.

-----------------
Modified 07/2002
By Ralph Giles <giles@ghostscript.com>
Still 100% public domain
modified for use with stdint types, autoconf
code cleanup, removed attribution comments
switched SHA1Final() argument order for consistency
use SHA1_ prefix for public api
move public api to sha1.h
*/

/*
Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

/* #define SHA1HANDSOFF  */

#include <stdio.h>
#include <string.h>


static void RD_SHA1_Transform(uint32_t state[5], const uint8_t buffer[64]);

#define RDrol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
/* FIXME: can we do this in an endian-proof way? */
#ifdef WORDS_BIGENDIAN
#define RDblk0(i) block->l[i]
#else
#define RDblk0(i) (block->l[i] = (RDrol(block->l[i],24)&0xFF00FF00) \
    |(RDrol(block->l[i],8)&0x00FF00FF))
#endif
#define RDblk(i) (block->l[i&15] = RDrol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+RDblk0(i)+0x5A827999+RDrol(v,5);w=RDrol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+RDblk(i)+0x5A827999+RDrol(v,5);w=RDrol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+RDblk(i)+0x6ED9EBA1+RDrol(v,5);w=RDrol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+RDblk(i)+0x8F1BBCDC+RDrol(v,5);w=RDrol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+RDblk(i)+0xCA62C1D6+RDrol(v,5);w=RDrol(w,30);

/* Hash a single 512-bit block. This is the core of the algorithm. */
static void RD_SHA1_Transform(uint32_t state[5], const uint8_t buffer[64])
{
    uint32_t a, b, c, d, e;
    typedef union {
        uint8_t c[64];
        uint32_t l[16];
    } CHAR64LONG16;
    CHAR64LONG16* block;

#ifdef SHA1HANDSOFF
    static uint8_t workspace[64];
    block = (CHAR64LONG16*)workspace;
    memcpy(block, buffer, 64);
#else
    block = (CHAR64LONG16*)buffer;
#endif

    /* Copy context->state[] to working vars */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
    R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
    R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
    R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
    R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
    R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
    R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
    R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
    R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
    R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
    R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
    R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
    R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
    R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
    R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
    R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
    R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
    R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
    R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
    R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);

    /* Add the working vars back into context.state[] */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;

    /* Wipe variables */
    a = b = c = d = e = 0;
}


/* SHA1Init - Initialize new context */
static void RD_SHA1_Init(RD_SHA1_CTX* context)
{
    /* SHA1 initialization constants */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}


/* Run your data through this. */
static void RD_SHA1_Update(RD_SHA1_CTX* context, const uint8_t* data, const unsigned long len)
{
    size_t i, j;

#ifdef VERBOSE
    SHAPrintContext(context, "before");
#endif

    j = (context->count[0] >> 3) & 63;
    if ((context->count[0] += len << 3) < (len << 3)) context->count[1]++;
    context->count[1] += (len >> 29);
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64-j));
        RD_SHA1_Transform(context->state, context->buffer);
        for ( ; i + 63 < len; i += 64) {
            RD_SHA1_Transform(context->state, data + i);
        }
        j = 0;
    }
    else i = 0;
    memcpy(&context->buffer[j], &data[i], len - i);

#ifdef VERBOSE
    SHAPrintContext(context, "after ");
#endif
}


/* Add padding and return the message digest. */
static void RD_SHA1_Final(uint8_t digest[RD_SHA1_DIGEST_SIZE],RD_SHA1_CTX* context)
{
    uint32_t i;
    uint8_t  finalcount[8];

    for (i = 0; i < 8; i++) {
        finalcount[i] = (unsigned char)((context->count[(i >= 4 ? 0 : 1)]
         >> ((3-(i & 3)) * 8) ) & 255);  /* Endian independent */
    }
    RD_SHA1_Update(context, (uint8_t *)"\200", 1);
    while ((context->count[0] & 504) != 448) {
        RD_SHA1_Update(context, (uint8_t *)"\0", 1);
    }
    RD_SHA1_Update(context, finalcount, 8);  /* Should cause a SHA1_Transform() */
    for (i = 0; i < RD_SHA1_DIGEST_SIZE; i++) {
        digest[i] = (uint8_t)
         ((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
    }

    /* Wipe variables */
    i = 0;
    memset(context->buffer, 0, 64);
    memset(context->state, 0, 20);
    memset(context->count, 0, 8);
    memset(finalcount, 0, 8);	/* SWR */

#ifdef SHA1HANDSOFF  /* make SHA1Transform overwrite its own static vars */
    SHA1_Transform(context->state, context->buffer);
#endif
}

#ifndef CNFGHTTPSERVERONLY

/*************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

char * cnfg_http_window_name;

int rd_request_fullscreen;

uint32_t * transitbuffer;
int transitlen;
int transitmax;

uint32_t * backbuffer;
int backbufferlen;
int backbuffermax;

uint32_t * datacmds;
int curdatacmds;
int maxdatacmds;

short last_dimensions_w;
short last_dimensions_h;

short cnfg_req_w;
short cnfg_req_h;


void HTTPCustomCallback( )
{
	if( curhttp->rcb )
		((void(*)())curhttp->rcb)();
	else
		curhttp->isdone = 1;
}


//Close of curhttp happened.
void CloseEvent()
{
}

static void readrdbuffer_websocket_dat(  int len )
{
	do
	{
		int bufferok = TCPCanSend( curhttp->socket, 1340 );
		if( transitlen <= 0 || !bufferok || curhttp->bytesleft == -1 ) return;

		int offset = transitlen * 4 - curhttp->bytesleft;
		uint8_t * offdata = ((uint8_t*)transitbuffer) + offset;
		int tosend = curhttp->bytesleft;
		if( tosend > 1340 ) tosend = 1340;

		WebSocketSend( offdata, tosend );

		//Special - we send an empty frame to indicate competion.
		if( tosend == 0 )
			curhttp->bytesleft = -1;
		else
			curhttp->bytesleft -= tosend;
	} while(1);
}



void ConsumeBackBufferForTransit()
{
	int len = transitlen;
	int maxv = transitmax;
	uint32_t  * dcmd = transitbuffer;

	transitbuffer = backbuffer;
	transitlen = backbufferlen;
	transitmax = backbuffermax;

	backbuffer = dcmd;
	backbufferlen = 0;
	backbuffermax = maxv;
}

static void readrdbuffer_websocket_cmd(  int len )
{
	uint8_t  buf[1300];
	int i;

	if( len > 1300 ) len = 1300;

	for( i = 0; i < len; i++ )
	{
		buf[i] = WSPOPMASK();
	}

	if( strncmp( buf, "SWAP", 4 ) == 0 )
	{
		last_dimensions_w = buf[4] | ( buf[5]<<8 );
		last_dimensions_h = buf[6] | ( buf[7]<<8 );
		ConsumeBackBufferForTransit();
		curhttp->bytesleft = transitlen * 4;
	}

	if( strncmp( buf, "MOTN", 4 ) == 0 )
	{
		int x = buf[4] | ( buf[5]<<8 );
		int y = buf[6] | ( buf[7]<<8 );
		int but = buf[11];
		HandleMotion( x, y, but );
	}

	if( strncmp( buf, "BUTN", 4 ) == 0 )
	{
		int x = buf[4] | ( buf[5]<<8 );
		int y = buf[6] | ( buf[7]<<8 );
		int down = buf[10];
		int but = buf[11];
		HandleButton( x, y, but, down );
	}

	if( strncmp( buf, "KEYB", 4 ) == 0 )
	{
		int key = buf[6];
		int down = buf[7];
		HandleKey( key, down );
	}
}





void NewWebSocket()
{
	if( strncmp( (const char*)curhttp->pathbuffer, "/d/ws/cmdbuf", 9 ) == 0 )
	{
		printf( "Got connection.\n" );
		curhttp->rcb = (void*)&readrdbuffer_websocket_dat;
		curhttp->rcbDat = (void*)&readrdbuffer_websocket_cmd;
	}
	else
	{
		curhttp->is404 = 1;
	}
}

void WebSocketTick()
{
	if( curhttp->rcb )
	{
		((void(*)())curhttp->rcb)();
	}
}

void WebSocketData( int len )
{
	if( curhttp->rcbDat )
	{
		((void(*)( int ))curhttp->rcbDat)(  len ); 
	}
}

void HTTPCustomStart( )
{

	curhttp->rcb = 0;
	curhttp->bytesleft = 0;

	curhttp->isfirst = 1;
	HTTPHandleInternalCallback();
}

void QueueCmds( uint32_t * toqueue, int numwords )
{
	int origcmds = curdatacmds;
	curdatacmds += numwords;
	if( curdatacmds > maxdatacmds )
	{
		datacmds = realloc( datacmds, curdatacmds*4 );
		maxdatacmds = curdatacmds;
	}
	memcpy( datacmds + origcmds, toqueue, numwords*4 );
}

int CNFGSetup( const char * WindowName, int w, int h ) 
{
	RunHTTP( 8888 );
	if( cnfg_http_window_name ) free( cnfg_http_window_name );
	cnfg_http_window_name = strdup( WindowName );
	datacmds = 0;
	backbuffer = 0;
	backbufferlen = 0;
	curdatacmds = 0;
	maxdatacmds = 0;
	rd_request_fullscreen = 0;
	cnfg_req_w = w;
	cnfg_req_h = h;
	return 0;
}

void CNFGSetupFullscreen( const char * WindowName, int screen_number ) 
{
	CNFGSetup( WindowName, -1, -1 );
	rd_request_fullscreen = 1;
}

int CNFGHandleInput()
{
	TickHTTP();
	return 1;
}

// command structure:
//  0: Continuation
//  1: Color
//  2: Pixel
//  3: SegmentStart
//  4: RectangleStart
//  5: PolyStart
//  6: PolyContinue
//  7: Blit Pixels
//  8: Clear frame
//  9: Swap Buffers  (Used as "end of frame")
//  a: CNFGSetLineWidth

uint32_t CNFGColor( uint32_t RGBA )
{
	uint32_t cmds[2] = { 0x10000000, RGBA };
	QueueCmds( cmds, 2 );
	return RGBA;
}

void CNFGTackPixel( short x1, short y1 )
{
	if( x1 < 0 || x1 > 0x3fff ) return;
	if( y1 < 0 || y1 > 0x3fff ) return;
	uint32_t cmds[1] = { 0x20000000 | (x1 & 0x3fff ) | ( ( y1 & 0x3fff ) << 14 ) };
	QueueCmds( cmds, 1 );
}

void CNFGTackSegment( short x1, short y1, short x2, short y2 )
{
	uint32_t cmds[3] = { 
		0x30000000,
		( (uint16_t)x1 ) | ( (uint16_t)y1 << 16 ),
		( (uint16_t)x2 ) | ( (uint16_t)y2 << 16 ) };
	QueueCmds( cmds, 3 );
}

void CNFGTackRectangle( short x1, short y1, short x2, short y2 )
{
	uint32_t cmds[3] = { 
		0x40000000,
		( (uint16_t)x1 ) | ( (uint16_t)y1 << 16 ),
		( (uint16_t)x2 ) | ( (uint16_t)y2 << 16 ) };
	QueueCmds( cmds, 3 );
}

void CNFGSetLineWidth( short width )
{
	uint32_t cmds[1] = { 0xa0000000 | width };
	QueueCmds( cmds, 1 );
}

void CNFGTackPoly( RDPoint * points, int verts )
{
	uint32_t * cmds = alloca( 4 * verts + 4 );
	int i;
	cmds[0] = 0x50000000 | verts;
	for( i = 0; i < verts; i++ )
	{
		uint16_t lx = points[i].x;
		uint16_t ly = points[i].y;
		cmds[i+1] = lx | ( ly << 16 );
	}
	QueueCmds( cmds, verts + 1 );
}

void CNFGClearFrame()
{
	int sl = strlen( cnfg_http_window_name );
	int blocks = ( sl + 8 + 8 ) / 4;
	uint32_t * cmds = alloca( blocks * 4 );
	cmds[0] = 0x80000000 | blocks | (rd_request_fullscreen<<8) | (sl<<16);
	cmds[1] = CNFGBGColor;
	cmds[2] = cnfg_req_w | ( cnfg_req_h << 16);
	memcpy( cmds+3, cnfg_http_window_name, sl + 1 );
	QueueCmds( cmds, blocks );
}

void CNFGSwapBuffers()
{
	uint32_t cmds[1] = { 0x90000000 };
	QueueCmds( cmds, 1 );

	// And swap.
	int len = curdatacmds;
	int maxv = maxdatacmds;
	uint32_t  * dcmd = datacmds;

	datacmds = backbuffer;
	curdatacmds = 0;
	maxdatacmds = backbuffermax;

	backbuffer = dcmd;
	backbufferlen = len;
	backbuffermax = maxv;
}

void CNFGBlitImage( uint32_t * data, int x, int y, int w, int h )
{
	if( w < 0  || h < 0 ) return;

	uint32_t * cmds = alloca( 4 * w * h + 8 );
	cmds[0] = 0x70000000 | ( w & 0x3fff ) | ( ( h & 0x3fff ) << 14 );
	cmds[1] =  ( x | ( y<<16) );
	int i;
	memcpy( cmds + 2, data, w * h * 4 );
	QueueCmds( cmds, w * h + 2 );
}

void CNFGGetDimensions( short * x, short * y )
{
	*x = last_dimensions_w;
	*y = last_dimensions_h;
}

#endif

#endif
