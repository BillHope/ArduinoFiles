

class Str
{
  private:
  int Pos;  
  public:
  char s[256]; // 255 chars + a zero
  Str();
  boolean AddChar(char c);
};

Str::Str()
{
  Pos = 0;
  s[0] = 0;
};

boolean Str::AddChar(char c) // handle of \n ??
{
  if(Pos < 255)
  {
    s[Pos++] = c;
    s[Pos] = 0;
    return true;
  }
  else return false;
};
 
 
