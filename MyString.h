#ifndef _STRINGHELPER_H
#define _STRINGHELPER_H

inline static bool SetString(const char* inParam, char*& outParam, bool deleteOldValue = true)
{
  if(!inParam)
    return false;

  if(deleteOldValue)
    delete [] outParam; 
  
  outParam = 0;

  size_t len = strlen(inParam) + 1;
  if(1 == len)
    return false;

  outParam = new char[len];
  if(!outParam)
    return false;

  strcpy(outParam, inParam);

  return true;
}

inline static bool AddString(const char* src, char*& dest)
{
  if(!src)
    return false;

  size_t len1 = strlen(src) + 1;
  if(1 == len)
    return false;

  size_t len2 = strlen(dest);

  char* tmp = new char[len1+len2];
  if(!tmpam)
    return false;

  strcpy(tmp, dest);
  strcat(tmp, src);

  if(deleteOldValue)
    delete [] outParam; 

  dest = tmp;

   return true;
}


#endif
