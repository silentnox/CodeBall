#pragma once

#include <string>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include "csimplesocket/ActiveSocket.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
//using namespace std;

using std::to_string;
using std::string;

//template <typename Type>
//string & AddPropLine( string & InStr, const string & PropName, Type Value ) {
//
//	InStr += PropName;
//	InStr += " : ";
//	InStr += to_string( Value );
//	InStr += '\n';
//
//	return InStr;
//}

inline string PackVec3( double x, double y, double z ) {
	return to_string( x ) + " " + to_string( y ) + " " + to_string( z );
}

inline string PackVec4( double x, double y, double z, double w ) {
	return to_string( x ) + " " + to_string( y ) + " " + to_string( z ) + " " + to_string( w );
}

inline string PackVec3( Vec3 in ) {
	return to_string( in.x ) + " " + to_string( in.y ) + " " + to_string( in.z );
}

inline string PackLine( Vec3 a, Vec3 b ) {
	return PackVec3( a ) + " " + PackVec3( b );
}

struct Strconv {
	string s;
	Strconv( int Int ) {
		s = to_string( Int );
	}
	Strconv( float Float ) {
		s = to_string( Float );
	}
};

struct Color {
	double r, g, b, a;
	Color() {

	}
	Color( double inR, double inG, double inB, double inA = 1.0 ) {

	}
};

class JsonNetwork {
public:
	typedef std::map<string, string> Object;
	Document Doc;
	//GenericDocument<UTF8<>, CrtAllocator> Doc;

	std::vector<Object> Objects;

	std::stringstream Stream;

	CActiveSocket mSocket;

	int Port = 13000;
	string Host = "127.0.0.1";

	JsonNetwork( int InPort = -1 ) {
		Doc.SetArray();

		Connect(InPort);
	}

	bool Connect( int InPort = -1 ) {
		if (InPort != -1) Port = InPort;

		mSocket.Initialize();
		mSocket.DisableNagleAlgoritm();

		if (!mSocket.Open( reinterpret_cast<const uint8*>(Host.c_str()), static_cast<int16>(Port) )) {
			return false;
		}
		return true;
	}

	void AddObject( const Object & InObj ) {
		Objects.emplace_back( InObj );

		//Value v;
		////GenericValue<UTF8<>, CrtAllocator> v;
		//v.SetObject();
		//for (auto it : InObj) {
		//	v.AddMember( Value( it.first, Doc.GetAllocator() ).Move(), Value( it.second, Doc.GetAllocator() ).Move(), Doc.GetAllocator() );
		//}
		//Doc.PushBack( v, Doc.GetAllocator() );

		//Stream << "{";
		//for (const auto & It : InObj) {
		//	Stream << It.first << ":" << It.second;
		//}
		//Stream << "}";
	}

	void AddCommand( const string & Cmd, const string & Args ) {
		Object obj;
		obj["Type"] = "Cmd";
		obj["Name"] = Cmd;
		obj["Arg"] = Args;
		AddObject( obj );
	}

	bool SendCommand( string Cmd ) {
		if (!mSocket.IsSocketValid()) return false;
		Cmd.push_back( '\n' );
		if (mSocket.Send( reinterpret_cast<const uint8*>(Cmd.c_str()), static_cast<int32_t>(Cmd.length()) ) < 0) {
			return false;
		}
		return true;
	}

	void Submit() {
		StringBuffer buffer;
		Writer<StringBuffer> writer( buffer );
		//Doc.Accept( writer );
		//SendCommand( buffer.GetString() );
		//Doc.Clear();
		//Doc.GetAllocator().Clear();
		//Doc.SetArray();
		//Doc.Erase( Doc.Begin(), Doc.End() );

		Document doc;
		doc.SetArray();

		Document::AllocatorType& alloc = doc.GetAllocator();

		for (const Object & it : Objects) {
			Value v;
			v.SetObject();
			for (const auto & it2 : it) {
				v.AddMember( Value( it2.first, alloc ).Move(), Value( it2.second, alloc ).Move(), alloc );
			}
			doc.PushBack( v, alloc );
		}

		doc.Accept( writer );
		SendCommand( buffer.GetString() );

		Objects.clear();
	}
};
