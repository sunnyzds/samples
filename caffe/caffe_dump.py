from __future__ import division
from __future__ import print_function
import argparse
import numpy as np
import math, copy
import matplotlib.pyplot as plt
import sys,os
import caffe
import caffe.proto.caffe_pb2 as caffe_pb2
import time
import datetime
from google.protobuf import text_format
from scipy import stats
import scipy.io as sio

np.set_printoptions(threshold='nan')
np.set_printoptions(suppress=True)

def parse_args():
    parser = argparse.ArgumentParser(
        description='find the pretrained caffe models int8 quantize scale value')
    parser.add_argument('--proto', dest='proto',
                        help="path to deploy prototxt.", type=str)
    parser.add_argument('--model', dest='model',
                        help='path to pretrained weights', type=str)
    parser.add_argument('--save', dest='save',
                        help='path to save model', type=str, default='')
    parser.add_argument('--mean', dest='mean',
                        help='value of mean', type=float, nargs=3)
    parser.add_argument('--norm', dest='norm',
                        help='value of normalize', type=float, nargs=1, default=1.0)                            
    parser.add_argument('--images', dest='images',
                        help='path to calibration images', type=str)
    parser.add_argument('--output', dest='output',
                        help='path to output calibration table file', type=str, default='calibration-dev.table')
    parser.add_argument('--group', dest='group',
                        help='enable the group scale', type=int, default=0)        
    parser.add_argument('--gpu', dest='gpu',
                        help='use gpu to forward', type=int, default=0)
    parser.add_argument('--trans', dest='trans',
                        help='transpose the weights', type=int, default=0)
    parser.add_argument('--dump', dest='dump',
                        help='dump the network', type=int, default=0)
    args = parser.parse_args()
    return args, parser


global args, parser
args, parser = parse_args()

def usage_info():   
    print("Input params is illegal..")
    print("try it again:\n python caffe-int8-scale-tools-dev.py -h")

def caffe_layer(params, layer_name):   
    for i, layer in enumerate(params.layer):
        if(layer.name == layer_name):
            return layer

def caffe_params(net_file):
    params = caffe_pb2.NetParameter()
    with open(net_file) as f:
        text_format.Merge(f.read(), params)
    return params

def layer_dump(net):
    print(">>layer_dump")

    print("<<layer_dump")

def weight_transpose1(net, params):
    print(">>weight_dump")
    for i, layer in enumerate(params.layer):
        print("["+str(i)+"]:"+str(layer.name)+":"+str(layer.type))
    print("params shape>>")
    for layer_name, param in net.params.iteritems():
        layer = caffe_layer(params, layer_name)
        #print(layer)
        if (layer.type == "PReLU" or layer.type == "LReLU" or layer.type == "ReLU"):
            print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape))
        else:
            print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape) + " bias_shape:" + str(param[1].data.shape))
    print("params shape<<")
    print("params>>")
    for k, v in net.params.items():        
        layer = caffe_layer(params, k)
        print("===" + k + "====")
        if (layer.type == "PReLU" or layer.type == "LReLU"):
            print("name:" + k + " weights:")
            #print(param[0].data)
        else:
            print("name:" + k + " weights:")
            #print(param[0].data)            
            print("bias:")
            #print(param[1].data)
            #if (k == "conv1"):
            #print(v[0].data.shape)
            #print(v[0].data.shape[0])                
            #print(v[0].data.shape[1])
            for i in range(0, v[0].data.shape[0]):
                for j in range(0, v[0].data.shape[1]):                    
                    #print("i:" + str(i) + "j:" + str(j))
                    #print(v[0].data[i][j])
                    v[0].data[i][j] = np.transpose(v[0].data[i][j])      
    print("params<<")
    print("<<weight_transpose")

def weight_transpose(net, params):
    print(">>weight_dump")
    for i, layer in enumerate(params.layer):
        print("["+str(i)+"]:"+str(layer.name)+":"+str(layer.type))
    print("params shape>>")
    for layer_name, param in net.params.iteritems():
        layer = caffe_layer(params, layer_name)
        #print(layer)
        print("==name:%s type:%s=="%(layer_name, layer.type))
        if (layer.type == "PReLU" or layer.type == "LReLU" or layer.type == "ReLU"):
            print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape))
        else:
            if (layer.type == "Convolution"):
                #print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape) + " bias_shape:" + str(param[1].data.shape))
                if layer.convolution_param is not None and layer.convolution_param.bias_term is not None:
                    #print(layer.convolution_param.bias_term)
                    if (layer.convolution_param.bias_term):
                        print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape) + " bias_shape:" + str(param[1].data.shape))
                    else:
                        print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape))
                else:
                        print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape) + " bias_shape:" + str(param[1].data.shape))
            else:
                print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape))
    print("params shape<<")
    print("params>>")
    for k, v in net.params.items():
        layer = caffe_layer(params, k)
        print("===" + k + "====")
        if (layer.type == "PReLU" or layer.type == "LReLU"):
            print(" weights:")
        else:            
            if (layer.type == "Convolution"):
                #print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape) + " bias_shape:" + str(param[1].data.shape))
                if layer.convolution_param is not None and layer.convolution_param.bias_term is not None:
                    #print(layer.convolution_param.bias_term)
                    if (layer.convolution_param.bias_term):
                        print(" weights:")
                        print(" bias:")
                    else:
                        print(" weights:")
                else:
                        print(" weights:")
                        print(" bias:")
                for i in range(0, v[0].data.shape[0]):
                    for j in range(0, v[0].data.shape[1]):                    
                        #print("i:" + str(i) + "j:" + str(j))
                        #print(v[0].data[i][j])
                        v[0].data[i][j] = np.transpose(v[0].data[i][j])  
            else:
                print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape))
                print(" weights:")           
    print("params<<")
    print("<<weight_dump")

def weight_dump(net, params):
    print(">>weight_dump")
    for i, layer in enumerate(params.layer):
        print("["+str(i)+"]:"+str(layer.name)+":"+str(layer.type))
    print("params shape>>")
    for layer_name, param in net.params.iteritems():
        layer = caffe_layer(params, layer_name)
        #print(layer)
        print("==name:%s type:%s=="%(layer_name, layer.type))
        if (layer.type == "PReLU" or layer.type == "LReLU" or layer.type == "ReLU"):
            print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape))
        else:
            if (layer.type == "Convolution"):
                #print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape) + " bias_shape:" + str(param[1].data.shape))
                if layer.convolution_param is not None and layer.convolution_param.bias_term is not None:
                    #print(layer.convolution_param.bias_term)
                    if (layer.convolution_param.bias_term):
                        print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape) + " bias_shape:" + str(param[1].data.shape))
                    else:
                        print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape))
                else:
                        print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape) + " bias_shape:" + str(param[1].data.shape))
            else:
                print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape))
    print("params shape<<")
    print("params>>")
    for k, v in net.params.items():
        layer = caffe_layer(params, k)
        f = open(k+"_param.txt", "w")
        print("===" + k + "====")
        print("%s %s:"%(k, layer.type), file=f)
        if (layer.type == "PReLU" or layer.type == "LReLU"):
            print(" weights:")
            print("weights(%s):"%(str(v[0].data.shape)), file=f)
            print(v[0].data, file=f)
        else:            
            if (layer.type == "Convolution"):
                #print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape) + " bias_shape:" + str(param[1].data.shape))
                if layer.convolution_param is not None and layer.convolution_param.bias_term is not None:
                    #print(layer.convolution_param.bias_term)
                    if (layer.convolution_param.bias_term):
                        print(" weights:")
                        print("weights(%s):"%(str(v[0].data.shape)), file=f)
                        print(v[0].data, file=f)
                        print(" bias:")
                        print("bias(%s):"%(str(v[1].data.shape)), file=f)
                        print(v[1].data, file=f)
                    else:
                        print(" weights:")
                        print("weights(%s):"%(str(v[0].data.shape)), file=f)
                        print(v[0].data, file=f)                        
                else:
                        print(" weights:")
                        print("weights(%s):"%(str(v[0].data.shape)), file=f)
                        print(v[0].data, file=f)
                        print(" bias:")
                        print("bias(%s):"%(str(v[1].data.shape)), file=f)
                        print(v[1].data, file=f)
            else:
                print("name:" + layer_name + " weights_shape:" + str(param[0].data.shape))
                print(" weights:")
                print("weights(%s):"%(str(v[0].data.shape)), file=f)
                print(v[0].data, file=f)
            #if (k == "conv1"):
                #print(v[0].data.shape)
                #print(v[0].data.shape[0])                
                #print(v[0].data.shape[1])
                #for i in range(0, v[0].data.shape[0]):
                    #for j in range(0, v[0].data.shape[1]):                    
                        #print("i:" + str(i) + "j:" + str(j))
                        #print(v[0].data[i][j])
                #print("xxxx" + str(v[0].data.shape) + "xxx" + str(v[1].data.shape))
                #print(v[0].data)
                #print(v[1].data)
                #print("000")
                #print(v[0].data[0][0])
                #t00 = np.transpose(v[0].data[0][0])
                #print("t000")
                #print(t00)
                #print("save back")
                #v[0].data[0][0] = t00
                #print(v[0].data[0][0])
                #print("001")
                #print(v[0].data[0][1])
                #sio.savemat('conv1_w', {'conv1_w':conv1_w})  
                #sio.savemat('conv1_b', {'conv1_b':conv1_b})
        f.close()
    print("params<<")
    print("<<weight_dump")

def blob_dump(net):
    print(">>blob_dump")
    for k, v in net.blobs.items():
        print("===%s==="%(k))
        print(v.data.shape)
    print("<<blob_dump")

def main():   
    # time start
    time_start = datetime.datetime.now()
    print(args)
    if args.proto == None or args.model == None:
        usage_info()
        return None
    # deploy caffe prototxt path
    net_file = args.proto
    # trained caffemodel path
    caffe_model = args.model
    save_model = args.save
    # initial caffe net and the forword model(GPU or CPU)
    print(">>setup network")
    net = caffe.Net(net_file,caffe_model,caffe.TEST)
    print("<<setup network")
    params = caffe_params(net_file)
    if args.dump == 1:        
        weight_dump(net, params)
        blob_dump(net)
    if save_model != "":
        print(">>save model")        
        net.save(save_model)
        print("<<save model")        
    if args.trans == 1:
        print(">>do the transpose")
        caffe_model_trans = caffe_model + "_trans"
        print(caffe_model_trans)
        weight_transpose(net, params)
        net.save(caffe_model_trans)
        print("<<do the transpose")

if __name__ == "__main__":
    main()
