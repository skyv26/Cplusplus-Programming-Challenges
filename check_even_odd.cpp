#include<iostream>
using namespace std;

int main(){
  int dummy_number;
  cout<<"\n\nEnter any number : ";
  cin>>dummy_number;

  if(dummy_number % 2 == 0)
    cout<<dummy_number<<" is Even !!"<<endl<<endl;
  else
    cout<<dummy_number<<" is Odd !!"<<endl<<endl;

  return 0;
}