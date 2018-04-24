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

#include "DistanceFieldGlyphCache.h"

#include "Magnum/ImageView.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#ifndef CORRADE_NO_ASSERT
#include "Magnum/GL/PixelFormat.h"
#endif
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/TextureTools/DistanceField.h"

namespace Magnum { namespace Text {

DistanceFieldGlyphCache::DistanceFieldGlyphCache(const Vector2i& originalSize, const Vector2i& size, const UnsignedInt radius):
    #if !(defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_TARGET_GLES2))
    GlyphCache(GL::TextureFormat::R8, originalSize, size, Vector2i(radius)),
    #elif !defined(MAGNUM_TARGET_WEBGL)
    /* Luminance is not renderable in most cases */
    GlyphCache(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>() ?
        GL::TextureFormat::Red : GL::TextureFormat::RGB, originalSize, size, Vector2i(radius)),
    #else
    GlyphCache(GL::TextureFormat::RGB, originalSize, size, Vector2i(radius)),
    #endif
    scale(Vector2(size)/Vector2(originalSize)), radius(radius)
{
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::texture_rg);
    #endif

    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* Luminance is not renderable in most cases */
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        Warning() << "Text::DistanceFieldGlyphCache:" << GL::Extensions::EXT::texture_rg::string() << "not supported, using inefficient RGB format for glyph cache texture";
    #endif
}

void DistanceFieldGlyphCache::setImage(const Vector2i& offset, const ImageView2D& image) {
    const GL::PixelFormat format = GL::pixelFormat(image.format());
    #if !(defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_TARGET_GLES2))
    const GL::TextureFormat internalFormat = GL::TextureFormat::R8;
    CORRADE_ASSERT(format == GL::PixelFormat::Red,
        "Text::DistanceFieldGlyphCache::setImage(): expected"
        << GL::PixelFormat::Red << "but got" << format, );
    #else
    GL::TextureFormat internalFormat;
    #ifndef MAGNUM_TARGET_WEBGL
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>()) {
        internalFormat = GL::TextureFormat::Red;
        CORRADE_ASSERT(format == GL::PixelFormat::Red,
            "Text::DistanceFieldGlyphCache::setImage(): expected"
            << GL::PixelFormat::Red << "but got" << format, );
    } else
    #endif
    {
        internalFormat = GL::TextureFormat::Luminance;
        CORRADE_ASSERT(format == GL::PixelFormat::Luminance,
            "Text::DistanceFieldGlyphCache::setImage(): expected"
            << GL::PixelFormat::Luminance << "but got" << format, );
    }
    #endif

    GL::Texture2D input;
    input.setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setMinificationFilter(GL::SamplerFilter::Linear)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setImage(0, internalFormat, image);

    /* Create distance field from input texture */
    TextureTools::distanceField(input, texture(), Range2Di::fromSize(offset*scale, image.size()*scale), radius, image.size());
}

void DistanceFieldGlyphCache::setDistanceFieldImage(const Vector2i& offset, const ImageView2D& image) {
    const GL::PixelFormat format = GL::pixelFormat(image.format());
    #if !(defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_TARGET_GLES2))
    CORRADE_ASSERT(format == GL::PixelFormat::Red,
        "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): expected" << GL::PixelFormat::Red << "but got" << format, );
    #else
    #ifndef MAGNUM_TARGET_WEBGL
    if(GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_rg>())
        CORRADE_ASSERT(format == GL::PixelFormat::Red,
            "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): expected" << GL::PixelFormat::Red << "but got" << format, );
    else
    #endif
    {
        /* Luminance is not renderable in most cases */
        CORRADE_ASSERT(format == GL::PixelFormat::RGB,
            "Text::DistanceFieldGlyphCache::setDistanceFieldImage(): expected" << GL::PixelFormat::RGB << "but got" << format, );
    }
    #endif

    texture().setSubImage(0, offset, image);
}

}}
