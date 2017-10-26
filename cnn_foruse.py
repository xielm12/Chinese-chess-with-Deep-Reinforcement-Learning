#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Aug 18 22:57:34 2017

@author: zty
"""

#cnn by zty: chess
import numpy as np
def cnn(state):    
    print('py called!')
    return 0

"""
import tensorflow as tf

LABEL_SIZE = 1  
VECTOR_SIZE = 1260
DATA_SIZE = (LABEL_SIZE + VECTOR_SIZE)
  
TRAIN_NUM = 116693
TRAIN_NUMS = 116693
TEST_NUM = 116693

model_path = "/home/zty/newest_chess/model/"

#regularization
def variable_with_weight_loss(shape, stddev, wl):
    var = tf.Variable(tf.truncated_normal(shape, stddev = stddev))
    if(wl is not None):
        weight_loss = tf.multiply(tf.nn.l2_loss(var), wl, name = 'weight_loss')
        tf.add_to_collection('losses', weight_loss)
    return var

#discounted average loss
def discounted_average_loss(i, avg, loss):
    if(i == 0):
        avg = 0
    if(i <= 50):
        return (avg * i + loss) / (i + 1)
    return avg * 0.98 + loss * 0.02

#regularized loss
def regularized_loss(loss):
    tf.add_to_collection('losses', loss)
    return tf.add_n(tf.get_collection('losses'), name = 'total_loss')

#helper functions
def weight_variable(shape):
    initial = tf.truncated_normal(shape, stddev = 0.1)
    return tf.Variable(initial)
    
def bias_variable(shape):
    initial = tf.constant(0.1, shape = shape)
    return tf.Variable(initial)
    
def conv(x, W):
    return tf.nn.conv2d(x, W, strides = [1,1,1,1], padding = 'SAME')
    
def pool_3(x):
    return tf.nn.max_pool(x, ksize = [1,3,3,1], strides = [1,1,1,1], padding = 'SAME')

def pool_2(x):
    return tf.nn.max_pool(x, ksize = [1,2,2,1], strides = [1,1,1,1], padding = 'SAME')

def cnn(state):    

    #reshape input to 4-d structured tensor, where 14 stand for channel
    s = tf.placeholder(tf.float32, [1260]) 
    x = tf.placeholder(tf.float32, [None, 1260])
    y_ = tf.placeholder(tf.float32, [None, 1])
    x = tf.reshape(s, [None, 1260])
    x_image = tf.reshape(x, [-1,10,9,14])
    
    #################### defining CNN architectures ####################  
    
    ############### primary varied receptive CNNs ###############
    
    ##### 3 * 3 conv layers #####
    W_conv1 = variable_with_weight_loss(shape = [3,3,14,48], stddev = 5e-2, wl = 0.0002)
    b_conv1 = bias_variable([48])
    h_conv1 = tf.nn.relu(conv(x_image, W_conv1) + b_conv1)  
    norm1 = tf.nn.lrn(h_conv1, 4, bias = 1.0, alpha = 0.001 / 9.0, beta = 0.75)
    
    ##### 5 * 5 conv layers #####
    W_conv2 = variable_with_weight_loss(shape = [5,5,14,48], stddev = 5e-2, wl = 0.0002)
    b_conv2 = bias_variable([48])
    h_conv2 = tf.nn.relu(conv(x_image, W_conv2) + b_conv2)   
    norm2 = tf.nn.lrn(h_conv2, 4, bias = 1.0, alpha = 0.001 / 9.0, beta = 0.75)
    
    ##### stacked 3 * 3 conv layers #####
    W_conv3 = variable_with_weight_loss(shape = [3,3,14,32], stddev = 5e-2, wl = 0.0002)
    b_conv3 = bias_variable([32])
    h_conv3 = tf.nn.relu(conv(x_image, W_conv3) + b_conv3)
    
    W_conv4 = variable_with_weight_loss(shape = [3,3,32,48], stddev = 5e-2, wl = 0.0002)
    b_conv4 = bias_variable([48])
    h_conv4 = tf.nn.relu(conv(h_conv3, W_conv4) + b_conv4)    
    norm4 = tf.nn.lrn(h_conv4, 4, bias = 1.0, alpha = 0.001 / 9.0, beta = 0.75)
    
    ##### 1 * 9 conv layers (row layers) #####
    W_conv5 = variable_with_weight_loss(shape = [1,9,14,48], stddev = 5e-2, wl = 0.0002)
    b_conv5 = bias_variable([48])
    h_conv5 = tf.nn.relu(conv(x_image, W_conv5) + b_conv5)    
    norm5 = tf.nn.lrn(h_conv5, 4, bias = 1.0, alpha = 0.001 / 9.0, beta = 0.75)
    
    ##### 10 * 1 conv layers (align layers) #####
    W_conv6 = variable_with_weight_loss(shape = [10,1,14,48], stddev = 5e-2, wl = 0.0002)
    b_conv6 = bias_variable([48])
    h_conv6 = tf.nn.relu(conv(x_image, W_conv6) + b_conv6)    
    norm6 = tf.nn.lrn(h_conv6, 4, bias = 1.0, alpha = 0.001 / 9.0, beta = 0.75)
    
    ############### subsequent GoogLeNet inception module ###############
    
    ##### concat layer #####
    
    concat = tf.concat([norm1, norm2, norm4, norm5, norm6], 3)
    
    ##### 3 * 3 pool + 1 * 1 conv #####
    
    h_pool7 = pool_3(concat)
    W_conv7 = variable_with_weight_loss(shape = [1,1,240,24], stddev = 5e-2, wl = 0.0002)
    b_conv7 = bias_variable([24])
    h_conv7 = tf.nn.relu(conv(h_pool7, W_conv7) + b_conv7) 
    
    ##### 2 * 2 pool + 1 * 1 conv #####
    
    h_pool8 = pool_2(concat)
    W_conv8 = variable_with_weight_loss(shape = [1,1,240,24], stddev = 5e-2, wl = 0.0002)
    b_conv8 = bias_variable([24])
    h_conv8 = tf.nn.relu(conv(h_pool8, W_conv8) + b_conv8)
    
    ##### 1 * 1 conv #####
    
    W_conv9 = variable_with_weight_loss(shape = [1,1,240,24], stddev = 5e-2, wl = 0.0002)
    b_conv9 = bias_variable([24])
    h_conv9 = tf.nn.relu(conv(concat, W_conv9) + b_conv9)
    
    ##### 1 * 1 conv + 3 * 3 conv #####
    
    W_conv10 = variable_with_weight_loss(shape = [1,1,240,24], stddev = 5e-2, wl = 0.0002)
    b_conv10 = bias_variable([24])
    h_conv10 = tf.nn.relu(conv(concat, W_conv10) + b_conv10)
    
    W_conv11 = variable_with_weight_loss(shape = [3,3,24,24], stddev = 5e-2, wl = 0.0002)
    b_conv11 = bias_variable([24])
    h_conv11 = tf.nn.relu(conv(h_conv10, W_conv11) + b_conv11)
    
    ##### 1 * 1 conv + 5 * 5 conv #####
    
    W_conv12 = variable_with_weight_loss(shape = [1,1,240,24], stddev = 5e-2, wl = 0.0002)
    b_conv12 = bias_variable([24])
    h_conv12 = tf.nn.relu(conv(concat, W_conv12) + b_conv12)
    
    W_conv13 = variable_with_weight_loss(shape = [5,5,24,24], stddev = 5e-2, wl = 0.0002)
    b_conv13 = bias_variable([24])
    h_conv13 = tf.nn.relu(conv(h_conv12, W_conv13) + b_conv13)
    
    ##### 1 * 1 conv + stacked 3 * 3 convs #####
    
    W_conv14 = variable_with_weight_loss(shape = [1,1,240,24], stddev = 5e-2, wl = 0.0002)
    b_conv14 = bias_variable([24])
    h_conv14 = tf.nn.relu(conv(concat, W_conv14) + b_conv14)
    
    W_conv15 = variable_with_weight_loss(shape = [3,3,24,24], stddev = 5e-2, wl = 0.0002)
    b_conv15 = bias_variable([24])
    h_conv15 = tf.nn.relu(conv(h_conv14, W_conv15) + b_conv15)
    
    W_conv16 = variable_with_weight_loss(shape = [3,3,24,24], stddev = 5e-2, wl = 0.0002)
    b_conv16 = bias_variable([24])
    h_conv16 = tf.nn.relu(conv(h_conv15, W_conv16) + b_conv16)
    
    ############### final advanced layers ###############
    
    ##### raw pooling from input #####
    
    raw_pool = tf.nn.max_pool(x_image, ksize = [1,2,2,1], strides = [1,2,2,1], padding = 'SAME')
        
    ##### concat layer #####
    
    h_flat1 = tf.reshape(h_conv7, [-1, 10 * 9 * 24])
    h_flat2 = tf.reshape(h_conv8, [-1, 10 * 9 * 24])
    h_flat3 = tf.reshape(h_conv9, [-1, 10 * 9 * 24])
    h_flat4 = tf.reshape(h_conv11, [-1, 10 * 9 * 24])
    h_flat5 = tf.reshape(h_conv13, [-1, 10 * 9 * 24])
    h_flat6 = tf.reshape(h_conv16, [-1, 10 * 9 * 24])
    h_flat7 = tf.reshape(raw_pool, [-1, 5 * 5 * 14])
    fc_concat = tf.concat([h_flat1, h_flat2, h_flat3, h_flat4, h_flat5, h_flat6, h_flat7], 1)
    
    ##### fc layer #####
    
    W_fc = variable_with_weight_loss(shape = [10 * 9 * 24 * 6 + 5 * 5 * 14, 256], stddev = 0.04, wl = 0.05)
    b_fc = bias_variable([256])
    h_fc = tf.nn.relu(tf.matmul(fc_concat, W_fc) + b_fc)
    
    ##### drop-out layer #####
    
    keep_prob = tf.placeholder(tf.float32)
    h_fc_drop = tf.nn.dropout(h_fc, keep_prob)
    
    ##### output layer #####
    
    W = variable_with_weight_loss(shape = [256, 1], stddev = 1/256.0, wl = 0.0002)
    b = bias_variable([1])
    y_conv = tf.matmul(h_fc_drop, W) + b

    #################### restore model ####################
    
    sess = tf.InteractiveSession()
    saver = tf.train.Saver() 
    tf.global_variables_initializer().run()
    
    #restore model
    saver.restore(sess, model_path)

    #################### session for use ####################
    
    state_value = sess.run(y_conv[0], feed_dict = {s:state, y_:[[0]], keep_prob:1.0})
    return state_value

"""
