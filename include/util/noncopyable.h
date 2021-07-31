#ifndef _ZXY_NONCOPYABLE_H
#define _ZXY_NONCOPYABLE_H

namespace zxy{

struct noncopyable{
#if __cplusplus >= 201103L
		noncopyable() = default;
		noncopyable(noncopyable const&) = delete;
		noncopyable& operator=(noncopyable const&) = delete;
		noncopyable(noncopyable&&) = default;
		noncopyable& operator=(noncopyable&&) = default;

#else
		noncopyable() { }
private:
		noncopyable(noncopyable const&){}
		void operator=(noncopyable const&){}
#endif //_cplusplus > 201103L
};

}

#endif //_ZXY_NONCOPYABLE_H

