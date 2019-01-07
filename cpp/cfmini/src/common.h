#ifndef _COMMON_H_
#define _COMMON_H_

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/message.h>

//namespace gflags = google;
namespace cfmini {

using namespace std;

void GlobalInit(int* pargc, char*** pargv);

};
#endif /*_COMMON_H_*/
