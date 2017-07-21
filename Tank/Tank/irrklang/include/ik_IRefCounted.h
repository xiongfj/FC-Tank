// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "irrKlang" library.
// For conditions of distribution and use, see copyright notice in irrKlang.h

#ifndef __I_IRRKLANG_IREFERENCE_COUNTED_H_INCLUDED__
#define __I_IRRKLANG_IREFERENCE_COUNTED_H_INCLUDED__

#include "ik_irrKlangTypes.h"

namespace irrklang
{
	//! Base class(基类) of most objects of the irrKlang.
	/** This class provides reference counting(参考计数) through(通过) the methods grab() and drop().
	It also is able to store a debug string(调试字符串) for every instance(实例) of an object.
	Most objects of irrKlang are derived(来自) from IRefCounted, and so they are reference counted.

	When you receive an object in irrKlang (for example an ISound using play2D() or
	play3D()), and you no longer need the object, you have 
	to call drop(). This will destroy the object, if grab() was not called
	in another part of you program, because this part still needs the object.
	Note, that you only don't need to call drop() for all objects you receive, it
	will be explicitely noted in the documentation.

	A simple example:

	If you want to play a sound, you may want to call the method
	ISoundEngine::play2D. You call
	ISound* mysound = engine->play2D("foobar.mp3", false, false true);
	If you no longer need the sound interface, call mysound->drop(). The 
	sound may still play on after this because the engine still has a reference
	to that sound, but you can be sure that it's memory will be released as soon
	the sound is no longer used.

	// 如果你要增加一个声音资源
	If you want to add a sound source, you may want to call a method
	ISoundEngine::addSoundSourceFromFile. You do this like
	ISoundSource* mysource = engine->addSoundSourceFromFile("example.jpg");
	You will not have to drop the pointer to the source, because
	sound sources are managed(管理) by the engine (it will live as long as the sound engine) and
	the documentation says so. 
	*/
	class IRefCounted
	{
	public:

		//! Constructor.
		IRefCounted()
			: ReferenceCounter(1)
		{
		}

		//! Destructor.
		virtual ~IRefCounted()
		{
		}

		//! Grabs(抓取，增加) the object. Increments(使..增加) the reference counter(计数器，就是 ReferenceCounter) by one(加一).
		//! Someone who calls grab() to an object, should later also call
		//! drop() to it. If an object never gets as much drop() as grab()
		//! calls, it will never be destroyed.
		//! The IRefCounted class provides(提供) a basic reference(引用) counting(计数) mechanism(原理)
		//! with its methods grab() and drop(). Most objects of irrklang
		//! are derived(来自) from IRefCounted, and so they are reference counted.
		//!
		//! When you receive an object in irrKlang (for example an ISound using play2D() or
		//! play3D()), and you no longer need the object, you have 
		//! to call drop(). This will destroy the object, if grab() was not called
		//! in another part of you program, because this part still needs the object.
		//! Note, that you only don't need to call drop() for all objects you receive, it
		//! will be explicitely noted in the documentation.
		//! 
		//! A simple example:
		//! 
		//! If you want to play a sound, you may want to call the method
		//! ISoundEngine::play2D. You call
		//! ISound* mysound = engine->play2D("foobar.mp3", false, false true);
		//! If you no longer need the sound interface(接口), call mysound->drop(). The 
		//! sound may still play on after this because the engine still has a reference(引用)
		//! to that sound, but you can be sure that it's memory will be released as soon
		//! the sound is no longer used.
		void grab() { ++ReferenceCounter;}

		//! When you receive an object in irrKlang (for example an ISound using play2D() or
		//! play3D()), and you no longer need the object, you have 
		//! to call drop(). This will destroy the object, if grab() was not called
		//! in another part of you program, because this part still needs the object.
		//! Note, that you only don't need to call drop() for all objects you receive, it
		//! will be explicitely(明确的) noted in the documentation.
		//! 
		//! A simple example:
		//! 
		//! If you want to play a sound, you may want to call the method
		//! ISoundEngine::play2D. You call
		//! ISound* mysound = engine->play2D("foobar.mp3", false, false true);
		//! If you no longer need the sound interface, call mysound->drop(). The 
		//! sound may still play on after this because the engine still has a reference
		//! to that sound, but you can be sure that it's memory will be released as soon
		//! the sound is no longer used.
		bool drop()
		{
			--ReferenceCounter;

			if (!ReferenceCounter)
			{
				delete this;
				return true;
			}

			return false;
		}

	private:

		ik_s32	ReferenceCounter;	// ik_s32 这类型的数据在 ik_irrKlangTypes.h 有typedef
	};

} // end namespace irr

#endif

