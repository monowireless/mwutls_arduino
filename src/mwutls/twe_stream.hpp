#pragma once 

/* Copyright (C) 2019-2020 Mono Wireless Inc. All Rights Reserved.
 * Released under MW-OSSLA-1J,1E (MONO WIRELESS OPEN SOURCE SOFTWARE LICENSE AGREEMENT). */

#include "twe_common.hpp"
#include <memory>

namespace TWE {
	class IStreamOut;
	class IStreamSpecial;
	class IStream_endl;

	/// <summary>
	/// special char handling (endl, etc...)
	/// </summary>
	class IStreamSpecial {
	public:
		virtual ~IStreamSpecial() = default;
		virtual IStreamOut& operator ()(IStreamOut& of) = 0;
	};


	/// <summary>
	/// function object to write to stream (serial or etc...)
	/// </summary>
	class IStreamOut {
	protected:
		IStreamOut() = default;
	public:
		virtual ~IStreamOut() = default;
		virtual IStreamOut& operator ()(const char_t c) = 0; //! () operator as a function object
		virtual IStreamOut& write_w(wchar_t c) { return *this; }
		inline IStreamOut& operator << (const char_t c) { return (*this)(c); } // should be on root class
		inline IStreamOut& operator << (const uint8_t c) { return (*this)(static_cast<char_t>(c)); } // should be on root class
		inline IStreamOut& operator << (const wchar_t c) { return write_w(c); } // should be on root class
		virtual inline IStreamOut& operator << (IStreamSpecial& sc) { return sc(*this); } // implement std::endl like object
		inline IStreamOut& operator << (const char* s) { // const char*
			while (*s != 0) operator ()((char_t)*s++);
			return *this;
		}
		template <int N>
		inline IStreamOut& operator << (const uint8_t(&s)[N]) { // const char*
			for (int i = 0; i < N; i++) operator ()(static_cast<char_t>(s[i]));
			return *this;
		}
		inline IStreamOut& operator << (const wchar_t* s) { // const char*
			while (*s != 0) write_w(*s++);
			return *this;
		}
		inline IStreamOut& operator << (std::nullptr_t ptr) { return *this; }
	};

	// wrapper object
	class IStreamOutWrapper : public IStreamOut {
		std::unique_ptr<IStreamOut> _sp;
	public:
		IStreamOutWrapper(IStreamOut* obj) : _sp(obj) {}
		~IStreamOutWrapper() {}
		void reset(IStreamOut* ptr) { _sp.reset(ptr); }
		IStreamOut& operator ()(char_t c) { return _sp->operator()(c); }
		IStreamOut& write_w(wchar_t c) { return _sp->write_w(c); }
	};

	// dummy object
	class PutCharNull : public IStreamOut {
	public:
		inline IStreamOut& operator ()(char_t c) { return *this; }
	};

	/// TWESERCMD::endl(=CRLF) handling
	class IStream_endl : public IStreamSpecial {
	private:
		uint8_t _u8Style;

	public:
		const uint8_t CRLF = 0;
		const uint8_t LF = 1;

		// set or get style
		uint8_t& style() { return _u8Style; }

		IStream_endl(uint8_t u8Style = 0) : _u8Style(u8Style) {}

		inline IStreamOut& operator ()(IStreamOut& of) override {
			if (!_u8Style) {
				of('\r');
			}
			of('\n');
			return of;
		}
	};

	/// object to read from stream (serial or etc...)
	class IStreamIn {
	public:
		virtual ~IStreamIn() = default;
		IStreamIn() = default;
		virtual int get_a_byte() = 0;
	};

	/// <summary>
	/// dummy get char class
	/// </summary>
	class GetCharNull : public IStreamIn {
	public:
		inline int get_a_byte() override {
			return -1;
		}
	};


	/// <summary>
	///  pre-created endl object.
	/// </summary>
	extern IStream_endl Endl;
	extern IStream_endl crlf;
}