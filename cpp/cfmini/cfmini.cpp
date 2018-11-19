
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/message.h>

#include "caffe.pb.h"

#include "common.h"

int main(int argc, char** argv)
{
    FLAGS_logtostderr = 1;
    gflags::InitGoogleLogging(argv[0]);
    LOG(INFO) << ">>main";



    DLOG(INFO) << ">>main";
    return 0;
}
