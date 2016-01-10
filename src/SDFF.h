#pragma once

typedef unsigned int SDFF_Char;
typedef std::vector<unsigned char> SDFF_PixelVector;

struct SDFF_Bitmap
{
  int width;
  int height;
  SDFF_PixelVector pixels;
};

enum SDFF_Error { SDFF_OK = 0, SDFF_FONT_ALREADY_EXISTS, SDFF_CHAR_ALREADY_EXISTS, SDFF_FONT_NOT_EXISTS, SDFF_FT_NEW_FACE_ERROR, SDFF_FT_SET_CHAR_SIZE_ERROR, SDFF_FT_LOAD_CHAR_ERROR };

//-------------------------------------------------------------------------------------------------

class SDFF_Glyph
{
public:
  float left;
  float top;
  float right;
  float bottom;
  float horiBearingX;
  float horiBearingY;
  float horiAdvance;
};

//-------------------------------------------------------------------------------------------------

class SDFF_Font
{
  friend class SDFF_Builder;

public:

  const SDFF_Glyph & getGlyph(SDFF_Char charcode);
  float getKerning(SDFF_Char leftChar, SDFF_Char rightChar);
  int getFalloff() { return falloff; }

private:
  struct CharPair
  {
    SDFF_Char left;
    SDFF_Char right;
    bool operator <(const CharPair & val) const { return val.left < left || (val.left == left && val.right < right); }
  };

  int falloff;
  std::map<SDFF_Char, SDFF_Glyph> glyphs;
  std::map<CharPair, float> kerning;
};

//-------------------------------------------------------------------------------------------------

class SDFF_Builder
{
public:
  SDFF_Builder();
  ~SDFF_Builder();

  SDFF_Error init(int sourceFontSize, int sdfFontSize, int falloff);
  SDFF_Error addFont(const char * fileName, int faceIndex, SDFF_Font * out_font);
  SDFF_Error addChar(SDFF_Font & font, SDFF_Char charCode);
  SDFF_Error addChars(SDFF_Font & font, SDFF_Char firstCharCode, SDFF_Char lastCharCode);
  SDFF_Error addChars(SDFF_Font & font, const char * charString);
  SDFF_Error composeTexture(SDFF_Bitmap & bitmap, bool powerOfTwo);

private:

  typedef std::map<SDFF_Char, SDFF_Bitmap> CharMap;
  
  struct FontData
  {
    FT_Face ftFace;
    CharMap chars;
  };

  typedef std::map<SDFF_Font *, FontData> FontMap;
  typedef std::vector<float> DistanceFieldVector;

  FT_Library ftLibrary;
  FontMap fonts;
  int sourceFontSize; 
  int sdfFontSize;
  int falloff;
  
  float createSdf(const FT_Bitmap & ftBitmap, int falloff, DistanceFieldVector & result);
  float createDf(const FT_Bitmap & ftBitmap, int falloff, bool invert, DistanceFieldVector & result);
  float createDf1(const FT_Bitmap & ftBitmap, int falloff, bool invert, DistanceFieldVector & result);
  void copyBitmap(const SDFF_Bitmap & srcBitmap, SDFF_Bitmap & destBitmap, int posX, int posY);
};