#include "GUIText.h"

namespace EngineCore
{
	GUIText::GUIText(const std::string &text,
					 const float fontSize,
					 const FontType &font,
					 const glm::vec2 &position,
					 const float maxLineLength,
					 const bool isCentered)
		: mTextString(text),
		  mFontSize(fontSize),
		  mTextMeshVao(0),
		  mVertexCount(0),
		  mColor(),
		  mPosition(position),
		  mLineMaxSize(maxLineLength),
		  mNumberOfLines(0),
		  mFont(font),
		  mIsCenteredText(isCentered)
	{
		// load text
	}

	void GUIText::Remove()
	{
		// remove text
	}

	const FontType &GUIText::GetFont() const
	{
		return mFont;
	}

	void GUIText::SetColor(const glm::vec3 &color)
	{
		mColor = color;
	}

	glm::vec3 GUIText::GetColor() const
	{
		return mColor;
	}

	int32_t GUIText::GetNumberOfLines() const
	{
		return mNumberOfLines;
	}

	glm::vec2 GUIText::GetPosition() const
	{
		return mPosition;
	}

	int32_t GUIText::GetMesh() const
	{
		return mTextMeshVao;
	}

	void GUIText::SetMeshInfo(const int32_t vao, const int32_t verticesCount)
	{
		mTextMeshVao = vao;
		mVertexCount = verticesCount;
	}

	int32_t GUIText::GetVertexCount() const
	{
		return mVertexCount;
	}

	float GUIText::GetFontSize() const
	{
		return mFontSize;
	}

	void GUIText::SetNumberOfLines(const int32_t number)
	{
		mNumberOfLines = number;
	}

	bool GUIText::IsCentered() const
	{
		return mIsCenteredText;
	}

	float GUIText::GetMaxLineSize() const
	{
		return mLineMaxSize;
	}

	std::string GUIText::GetTextString() const
	{
		return mTextString;
	}
}
