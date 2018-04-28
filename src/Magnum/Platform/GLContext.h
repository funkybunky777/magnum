#ifndef Magnum_Platform_GLContext_h
#define Magnum_Platform_GLContext_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class @ref Magnum::Platform::GLContext
 */

#include <Corrade/Utility/Debug.h>

#include "Magnum/Context.h"
#include "Magnum/OpenGL.h"

namespace Magnum { namespace Platform {

/**
@brief Platform-specific OpenGL context

In most cases not needed to be used directly as the initialization is done
automatically in `*Application` classes. See @ref platform for more
information.
*/
class GLContext: public GL::Context {
    public:
        /**
         * @brief Constructor
         *
         * Parses command-line arguments, loads OpenGL function pointers using
         * platform-specific API, does initial setup, detects available
         * features and enables them throughout the engine. If detected version
         * is unsupported or any other error occurs, a message is printed to
         * output and the application exits. See @ref Context(NoCreateT, Int, char**)
         * and @ref tryCreate() for an alternative.
         * @see @ref Context-command-line, @fn_gl{Get} with @def_gl{MAJOR_VERSION},
         *      @def_gl{MINOR_VERSION}, @def_gl{CONTEXT_FLAGS},
         *      @def_gl{NUM_EXTENSIONS}, @fn_gl{GetString} with @def_gl{EXTENSIONS}
         */
        explicit GLContext(Int argc, const char** argv): GLContext{GL::NoCreate, argc, argv} { create(); }

        /** @overload */
        explicit GLContext(Int argc, char** argv): GLContext{argc, const_cast<const char**>(argv)} {}

        /** @overload */
        explicit GLContext(Int argc, std::nullptr_t argv): GLContext{argc, static_cast<const char**>(argv)} {}

        /**
         * @brief Default constructor
         *
         * Equivalent to passing @cpp {0, nullptr} @ce to
         * @ref GLContext(Int, const char**). Even if the command-line options
         * are not propagated, it's still possible to affect the renderer
         * behavior from the environment. See @ref Context-command-line for
         * more information.
         */
        explicit GLContext(): GLContext{0, nullptr} {}

        /**
         * @brief Construct the class without doing complete setup
         *
         * Unlike @ref Context(Int, char**) just parses command-line arguments
         * and sets @ref version() to @ref Version::None, everything else is
         * left in empty state. Use @ref create() or @ref tryCreate() to
         * complete the setup.
         */
        explicit GLContext(GL::NoCreateT, Int argc, const char** argv):
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            Magnum::Context{GL::NoCreate, argc, argv, flextGLInit} {}
            #else
            Magnum::Context{GL::NoCreate, argc, argv, nullptr} {}
            #endif

        /** @overload */
        explicit GLContext(GL::NoCreateT, Int argc, char** argv): GLContext{GL::NoCreate, argc, const_cast<const char**>(argv)} {}

        /** @overload */
        explicit GLContext(GL::NoCreateT, Int argc, std::nullptr_t argv): GLContext{GL::NoCreate, argc, static_cast<const char**>(argv)} {}

        /**
         * @brief Construct the class without doing complete setup
         *
         * Equivalent to passing @cpp {NoCreate, 0, nullptr} @ce to
         * @ref Context(NoCreateT, Int, const char**). Even if the command-line
         * options are not propagated, it's still possible to affect the
         * renderer behavior from the environment. See @ref Context-command-line
         * for more information.
         */
        explicit GLContext(GL::NoCreateT): GLContext{GL::NoCreate, 0, nullptr} {}

        /**
         * @brief Complete the context setup and exit on failure
         *
         * Finalizes the setup after the class was created using
         * @ref Context(NoCreateT, Int, char**). If detected version is
         * unsupported or any other error occurs, a message is printed to error
         * output and the application exits. See @ref Context(Int, char**) for
         * more information and @ref tryCreate() for an alternative.
         */
        void create() { return Magnum::GL::Context::create(); }

        /**
         * @brief Complete the context setup
         *
         * Unlike @ref create() just prints a message to error output and
         * returns `false` on error.
         */
        bool tryCreate() { return Magnum::GL::Context::tryCreate(); }
};

}}

#endif
