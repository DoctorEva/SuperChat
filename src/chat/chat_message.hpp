#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>

class chat_message
{
public:
  enum { header_length = 4 };
  enum { max_body_length = 512 };
  enum { max_username = 24 };

  char username[max_username];//This is the username that is to be used

  //This function sets the username for a user
  void set_username(char *name){
    std::memcpy(this->username,name,max_username);
    username_length=std::strlen(name);
  }

  chat_message()
    : body_length_(0)
  {
  }

  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  std::size_t length() const
  {
    return header_length + body_length_+username_length;
  }

  const char* body() const
  {
    return data_ + header_length+username_length+4;
  }

  char* body()
  {
    return data_ + header_length +username_length+4;
  }

  std::size_t body_length() const
  {
    return body_length_;
  }

  void body_length(std::size_t new_length)
  {
    body_length_ = new_length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool decode_header()
  {
    char header[header_length + 1] = "";
    std::strncat(header, data_, header_length);
    body_length_ = std::atoi(header);
    
    //Added finding the username that is sent
    char username_size[5]="";

    std::memcpy(username_size,&data_[4],4);


    username_length = std::atoi(username_size);


    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void encode_header()
  {
    char header[header_length+max_username+4 + 1] = ""; 
    std::sprintf(header, "%4d%4d%s", static_cast<int>(body_length_),static_cast<int>(username_length),username);
    std::memcpy(data_, header, header_length+4+username_length);
    
  }

  std::size_t get_username_length(){
    return username_length;
  }
private:

  char data_[header_length + max_body_length+max_username];
  std::size_t username_length;//This is the username length
  std::size_t body_length_;
};

#endif // CHAT_MESSAGE_HPP
