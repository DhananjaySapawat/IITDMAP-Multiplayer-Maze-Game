#include <iostream>

void function(char msg[]){
  std::cout << msg << "\n";
  std::string a = &msg[0];
  if(a == "hi"){
    std::cout << "hi\n";
  }
  else{
    std::cout << "hello\n";
  }
}

int main(){
  char msg[] = "hi";
  function(msg);
  return 0;
}
