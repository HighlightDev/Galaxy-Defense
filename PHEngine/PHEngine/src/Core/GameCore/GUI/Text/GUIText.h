#pragma once

#include "FontType.h"

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace EngineCore
{
	/**
	 * Represents a piece of text in the game.
	 */
	class GUIText
	{

		std::string mTextString;

		float mFontSize;

		int mTextMeshVao;

		int mVertexCount;

		glm::vec3 mColor;

		glm::vec2 mPosition;

		float mLineMaxSize;

		int mNumberOfLines;

		bool mIsCenteredText;

	public:
		/**
		 * Creates a new text, loads the text's quads into a VAO, and adds the text
		 * to the screen.
		 *
		 * @param text
		 *            - the text.
		 * @param fontSize
		 *            - the font size of the text, where a font size of 1 is the
		 *            default size.
		 * @param position
		 *            - the position on the screen where the top left corner of the
		 *            text should be rendered. The top left corner of the screen is
		 *            (0, 0) and the bottom right is (1, 1).
		 * @param maxLineLength
		 *            - basically the width of the virtual page in terms of screen
		 *            width (1 is full screen width, 0.5 is half the width of the
		 *            screen, etc.) Text cannot go off the edge of the page, so if
		 *            the text is longer than this length it will go onto the next
		 *            line. When text is centered it is centered into the middle of
		 *            the line, based on this line length value.
		 * @param centered
		 *            - whether the text should be centered or not.
		 */
		GUIText(const std::string &text,
				const float fontSize,
				const glm::vec2 &position,
				const float maxLineLength,
				const bool isCentered);

		/**
		 * Remove the text from the screen.
		 */
		void Remove();

		/**
		 * Set the color of the text.
		 *
		 * @param r
		 *            - red value, between 0 and 1.
		 * @param g
		 *            - green value, between 0 and 1.
		 * @param b
		 *            - blue value, between 0 and 1.
		 */
		void SetColor(const glm::vec3 &color);

		/**
		 * @return the colour of the text.
		 */
		glm::vec3 GetColor() const;

		/**
		 * @return The number of lines of text. This is determined when the text is
		 *         loaded, based on the length of the text and the max line length
		 *         that is set.
		 */
		int32_t GetNumberOfLines() const;

		/**
		 * @return The position of the top-left corner of the text in screen-space.
		 *         (0, 0) is the top left corner of the screen, (1, 1) is the bottom
		 *         right.
		 */
		glm::vec2 GetPosition() const;

		/**
		 * @return the ID of the text's VAO, which contains all the vertex data for
		 *         the quads on which the text will be rendered.
		 */
		int32_t GetMesh() const;

		/**
		 * Set the VAO and vertex count for this text.
		 *
		 * @param vao
		 *            - the VAO containing all the vertex data for the quads on
		 *            which the text will be rendered.
		 * @param verticesCount
		 *            - the total number of vertices in all of the quads.
		 */
		void SetMeshInfo(const int32_t vao, const int32_t verticesCount);

		/**
		 * @return The total number of vertices of all the text's quads.
		 */
		int32_t GetVertexCount() const;

		/**
		 * @return the font size of the text (a font size of 1 is normal).
		 */
		float GetFontSize() const;

		/**
		 * Sets the number of lines that this text covers (method used only in
		 * loading).
		 *
		 * @param number
		 */
		void SetNumberOfLines(const int32_t number);

		/**
		 * @return {@code true} if the text should be centered.
		 */
		bool IsCentered() const;

		/**
		 * @return The maximum length of a line of this text.
		 */
		float GetMaxLineSize() const;

		/**
		 * @return The string of text.
		 */
		std::string GetTextString() const;
	};
}
