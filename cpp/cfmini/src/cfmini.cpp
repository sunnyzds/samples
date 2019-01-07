#include "common.h"
#include "caffe.pb.h"
#include "io.h"

#include <stdio.h>
#include <limits.h>
#include <math.h>

#include <fstream>
#include <set>
#include <limits>
#include <map>
#include <algorithm>

using namespace std;

DEFINE_string(gpu, "",
    "Optional; run in GPU mode on given device IDs separated by ','."
    "Use '-gpu all' to run on all available GPUs. The effective training "
    "batch size is multiplied by the number of devices.");
DEFINE_string(solver, "",
    "The solver definition protocol buffer text file.");
DEFINE_string(model, "",
    "The model definition protocol buffer text file.");
DEFINE_string(phase, "",
    "Optional; network phase (TRAIN or TEST). Only used for 'time'.");
DEFINE_int32(level, 0,
    "Optional; network level.");
DEFINE_string(stage, "",
    "Optional; network stages (not to be confused with phase), "
    "separated by ','.");
DEFINE_string(snapshot, "",
    "Optional; the snapshot solver state to resume training.");
DEFINE_string(weights, "",
    "Optional; the pretrained weights to initialize finetuning, "
    "separated by ','. Cannot be set simultaneously with snapshot.");
DEFINE_int32(iterations, 50,
    "The number of iterations to run.");
DEFINE_string(sigint_effect, "stop",
             "Optional; action to take when a SIGINT signal is received: "
              "snapshot, stop or none.");
DEFINE_string(sighup_effect, "snapshot",
             "Optional; action to take when a SIGHUP signal is received: "
             "snapshot, stop or none.");

static bool read_proto_from_text(const char* filepath, google::protobuf::Message* message)
{
    std::ifstream fs(filepath, std::ifstream::in);
    if (!fs.is_open())
    {
        fprintf(stderr, "open failed %s\n", filepath);
        return false;
    }

    google::protobuf::io::IstreamInputStream input(&fs);
    bool success = google::protobuf::TextFormat::Parse(&input, message);

    fs.close();

    return success;
}

static bool read_proto_from_binary(const char* filepath, google::protobuf::Message* message)
{
    std::ifstream fs(filepath, std::ifstream::in | std::ifstream::binary);
    if (!fs.is_open())
    {
        fprintf(stderr, "open failed %s\n", filepath);
        return false;
    }

    google::protobuf::io::IstreamInputStream input(&fs);
    google::protobuf::io::CodedInputStream codedstr(&input);

    codedstr.SetTotalBytesLimit(INT_MAX, INT_MAX / 2);

    bool success = message->ParseFromCodedStream(&codedstr);

    fs.close();

    return success;
}

int main(int argc, char** argv)
{
    FLAGS_logtostderr = 1;    
    cfmini::GlobalInit(&argc, &argv);    
    LOG(INFO) << ">>main";

    ::google::SetUsageMessage("command line brew\n"
      "usage: caffe <command> <args>\n\n"
      "commands:\n"
      "  train           train or finetune a model\n"
      "  test            score a model\n"
      "  device_query    show GPU diagnostic information\n"
      "  time            benchmark model execution time");

    if (FLAGS_model.empty() || FLAGS_weights.empty()) {
        LOG(ERROR) << "model prototxt or weights is empy";
        return 0;
    }
    LOG(INFO) << "Model proto:[" << FLAGS_model << "] weights:[" << FLAGS_weights << "]";

    caffe::NetParameter proto;
    caffe::NetParameter net;

    // load
    cfmini::ReadProtoFromTextFileOrDie(FLAGS_model, &proto);
    //LOG(INFO) << "model defination:" << std::endl << proto.DebugString();
    cfmini::ReadProtoFromBinaryFileOrDie(FLAGS_weights, &net);

    // rename mapping for identical bottom top style
    std::map<std::string, std::string> blob_name_decorated;

    // bottom blob reference
    std::map<std::string, int> bottom_reference;

    // global definition line
    // [layer count] [blob count]
    int layer_count = proto.layer_size();
    LOG(INFO) << "layer cout: " << layer_count << std::endl;
    std::set<std::string> blob_names;
    for (int i=0; i<layer_count; i++) {
        const caffe::LayerParameter& layer = proto.layer(i);
        LOG(INFO) << "layer:[" << i << "][" << layer.type() << "]:[" << layer.name() <<"]";        
        for (int j=0; j<layer.bottom_size(); j++) {
            std::string blob_name = layer.bottom(j);            
            LOG(INFO) << layer.name() << "<-" << blob_name << std::endl;
            if (blob_name_decorated.find(blob_name) != blob_name_decorated.end()) {
                blob_name = blob_name_decorated[blob_name];
            }
            blob_names.insert(blob_name);
            if (bottom_reference.find(blob_name) == bottom_reference.end()) {
                bottom_reference[blob_name] = 1;
             } else {
                bottom_reference[blob_name] = bottom_reference[blob_name] + 1;
            }
        }

        if (layer.bottom_size() == 1 && layer.top_size() == 1 && layer.bottom(0) == layer.top(0)) {
            std::string blob_name = layer.top(0) + "_" + layer.name();
            LOG(INFO) << "top is same as bottom: [" << layer.top(0) << "]";
            LOG(INFO) << layer.name() << "->" << blob_name << std::endl;
            blob_name_decorated[layer.top(0)] = blob_name;
            blob_names.insert(blob_name);
        } else {
            for (int j=0; j<layer.top_size(); j++) {
                std::string blob_name = layer.top(j);
                LOG(INFO) << layer.name() << "->" << blob_name << std::endl;
                blob_names.insert(blob_name);
            }
        }
    }

    LOG(INFO) << "All blobs:";
    std::set<string>::iterator iter = blob_names.begin();
    while (iter!=blob_names.end()) {
        LOG(INFO) << "blobs:[" << *iter << "]";
        ++iter;
    }

    LOG(INFO) << "Bottom references:";
    int splitncnn_blob_count = 0;
    std::map<std::string, int>::iterator it = bottom_reference.begin();
    while (it != bottom_reference.end()) {
        LOG(INFO) << "bottom:[" << it->first << "] [" << it->second << "]";
        ++it;
    }

    LOG(INFO) << "Weights:" << std::endl;
    int netidx;
    for (netidx=0; netidx<net.layer_size(); netidx++) {        
        const caffe::LayerParameter& binlayer = net.layer(netidx);        
        LOG(INFO) << "Weight:[" <<netidx << "][" << binlayer.type() << "]:[" << binlayer.name() <<"]";        
    }

    LOG(INFO) << "Usage:";
    //::google::ShowUsageWithFlags(argv[0]);
    //gflags::ShowUsageWithFlagsRestrict(argv[0], "src");
    DLOG(INFO) << "<<main";
    return 0;
}
