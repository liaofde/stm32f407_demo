#ifndef __TESTCPP_H__
#define __TESTCPP_H__

#ifdef __cplusplus
 extern "C" {
#endif
  
class xxx
{
public:
  int a;
public:
  int add(int i,int j)
  {
    return i+j;
  }
};

void testcls_show(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif