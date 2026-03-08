#include <iostream>
#include <vector>
#include "derangement.hxx"

using D = Discrete::Derangement<long long>;

static const std::vector<long long> ref = {
  1,
  0,
  1,
  2,
  9,
  44,
  265,
  1854,
  14833,
  133496,
  1334961,
  14684570,
  176214841,
  2290792932LL,
  32071101049LL,
  481066515734LL,
  7697064251745LL,
  130850092279664LL,
  2355301661033953LL,
  44750731559645106LL,
  895014631192902121LL
};

int main(){
  bool ok=true;

  for(size_t n=0;n<ref.size();++n){
    auto v=D::calc(n);
    if(v!=ref[n]){
      std::cout<<"FAIL n="<<n<<" got="<<v<<" expect="<<ref[n]<<"\n";
      ok=false;
    }
  }

  std::cout<<(ok?"ALL SMALL TESTS OK\n":"SMALL TEST FAILED\n");

  for(int n=5;n<=25;++n){
    auto v=D::calc(n);
    std::cout<<"! "<<n<<" = "<<v<<"\n";
  }

  std::vector<int> a={0,1,2,3};
  auto perms=D{}.derange(a);
  std::cout<<"Derangements of {0,1,2,3}: "<<perms.size()<<"\n";
  for(auto& p:perms){
    for(auto x:p) std::cout<<x<<" ";
    std::cout<<"\n";
  }
}
