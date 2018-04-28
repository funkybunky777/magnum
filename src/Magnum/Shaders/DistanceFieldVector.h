#ifndef Magnum_Shaders_DistanceFieldVector_h
#define Magnum_Shaders_DistanceFieldVector_h
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
 * @brief Class @ref Magnum::Shaders::DistanceFieldVector, typedef @ref Magnum::Shaders::DistanceFieldVector2D, @ref Magnum::Shaders::DistanceFieldVector3D
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shaders/AbstractVector.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Distance field vector shader

Renders vector graphics in form of signed distance field. See
@ref TextureTools::distanceField() for more information. Note that the final
rendered outlook will greatly depend on radius of input distance field and
value passed to @ref setSmoothness(). You need to provide @ref Position and
@ref TextureCoordinates attributes in your triangle mesh and call at least
@ref setTransformationProjectionMatrix(), @ref setColor() and
@ref bindVectorTexture().

@image html shaders-distancefieldvector.png

@section Shaders-DistanceFieldVector-usage Example usage

Common mesh setup:

@snippet MagnumShaders.cpp DistanceFieldVector-usage1

Common rendering setup:

@snippet MagnumShaders.cpp DistanceFieldVector-usage2

@see @ref shaders, @ref DistanceFieldVector2D, @ref DistanceFieldVector3D
@todo Use fragment shader derivations to have proper smoothness in perspective/
    large zoom levels, make it optional as it might have negative performance
    impact
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT DistanceFieldVector: public AbstractVector<dimensions> {
    public:
        explicit DistanceFieldVector();

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         */
        explicit DistanceFieldVector(GL::NoCreateT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : AbstractVector<dimensions>{GL::NoCreate}
            #endif
            {}

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         */
        DistanceFieldVector& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix) {
            GL::AbstractShaderProgram::setUniform(_transformationProjectionMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set fill color
         * @return Reference to self (for method chaining)
         *
         * @see @ref setOutlineColor()
         */
        DistanceFieldVector& setColor(const Color4& color) {
            GL::AbstractShaderProgram::setUniform(_colorUniform, color);
            return *this;
        }

        /**
         * @brief Set outline color
         * @return Reference to self (for method chaining)
         *
         * @see @ref setOutlineRange(), @ref setColor()
         */
        DistanceFieldVector& setOutlineColor(const Color4& color) {
            GL::AbstractShaderProgram::setUniform(_outlineColorUniform, color);
            return *this;
        }

        /**
         * @brief Set outline range
         * @return Reference to self (for method chaining)
         *
         * Parameter @p start describes where fill ends and possible outline
         * starts. Initial value is @cpp 0.5f @ce, larger values will make the
         * vector art look thinner, smaller will make it look thicker.
         *
         * Parameter @p end describes where outline ends. If set to value
         * larger than @p start the outline is not drawn. Initial value is
         * @cpp 1.0f @ce.
         *
         * @see @ref setOutlineColor()
         */
        DistanceFieldVector& setOutlineRange(Float start, Float end) {
            GL::AbstractShaderProgram::setUniform(_outlineRangeUniform, Vector2(start, end));
            return *this;
        }

        /**
         * @brief Set smoothness radius
         * @return Reference to self (for method chaining)
         *
         * Larger values will make edges look less aliased (but blurry),
         * smaller values will make them look more crisp (but possibly
         * aliased). Initial value is @cpp 0.04f @ce.
         */
        DistanceFieldVector& setSmoothness(Float value) {
            GL::AbstractShaderProgram::setUniform(_smoothnessUniform, value);
            return *this;
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Overloads to remove WTF-factor from method chaining order */
        DistanceFieldVector<dimensions>& bindVectorTexture(GL::Texture2D& texture) {
            AbstractVector<dimensions>::bindVectorTexture(texture);
            return *this;
        }
        #ifdef MAGNUM_BUILD_DEPRECATED
        CORRADE_DEPRECATED("use bindVectorTexture() instead") DistanceFieldVector<dimensions>& setVectorTexture(GL::Texture2D& texture) {
            return bindVectorTexture(texture);
        }
        #endif
        #endif

    private:
        Int _transformationProjectionMatrixUniform{0},
            _colorUniform{1},
            _outlineColorUniform{2},
            _outlineRangeUniform{3},
            _smoothnessUniform{4};
};

/** @brief Two-dimensional distance field vector shader */
typedef DistanceFieldVector<2> DistanceFieldVector2D;

/** @brief Three-dimensional distance field vector shader */
typedef DistanceFieldVector<3> DistanceFieldVector3D;

}}

#endif
