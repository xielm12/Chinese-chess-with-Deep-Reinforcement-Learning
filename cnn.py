#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Aug 18 22:57:34 2017

@author: zty
"""

#cnn by zty: chess

import tensorflow as tf
import numpy as np
import os
import time

LABEL_SIZE = 1  
VECTOR_SIZE = 1260
DATA_SIZE = (LABEL_SIZE + VECTOR_SIZE)
  
TRAIN_NUM = 116693
TRAIN_NUMS = 116693
TEST_NUM = 116693

model_path = "/home/zty/newest_chess/model/"

#read data
def extract_data(filenames):   
    
    labels = None
    vectors = None

    for f in filenames:
        
        bytestream = open(f, 'rb') 
        buf = bytestream.read(TRAIN_NUM * DATA_SIZE * 4)
        
        #把数据流转化为np的数组,为什么要转化为np数组呢，因为array数组只支持一维操作，
        #为了满足我们的操作需求，我们利用np.frombuffer()将buf转化为numpy数组现在data的
        #shape为（30730000，），3073是3*1024+1得到的，3个channel（r，g，b），
        #每个channel有1024=32*32个信息，再加上 1 个label
        data = np.frombuffer(buf, dtype = np.int32)

        #改变数据格式,将shape从原来的（1261000，）——>为（1000，1261）
        data = data.reshape(TRAIN_NUM, DATA_SIZE)

        #分割数组,分割数组，np.hsplit是在水平方向上，将数组分解为label_size的一部分和
        #剩余部分两个数组，在这里label_size = 1，也就是把标签label给作为一个数组单独切分
        #出来如果你对np.split还不太了解，可以自行查阅一下，此时label_vectors的shape
        #应该是这样的[array([.......]) , array([.......................])]   
        labels_tmp = np.hsplit(data, [LABEL_SIZE])

        #此时labels[0]就是我们上面切分数组得到的第一个数组，在这里就是label数组，
        #这时的shape为array([[3] , [6] , [4] , ....... ,[7]])，我们把它reshape（）
        #一下变为了array([3 , 6 , ........ ,7])  
        label  = labels_tmp[0].reshape(TRAIN_NUM)
        
        #此时labels[1]就是我们上面切分数组的剩余部分，也就是图片部分我们把它reshape（）
        #为（1000，1260） 
        vector = labels_tmp[1].reshape(TRAIN_NUM, VECTOR_SIZE)
        
        if labels is None:
            labels = label
            vectors = vector
        else:
            #合并数组，不能用加法
            labels = np.concatenate((labels, label))
            vectors = np.concatenate((vectors, vector))
        
        #trial output for debug
        print("label sample = ", labels[3], "; vector sample = ", vectors[3])
        
    return labels, vectors


#定义提取训练数据函数
def extract_train_data(files_dir):
    #获得训练数据
    filenames = [os.path.join(files_dir, 'train.bin')]
    return extract_data(filenames)


#定义提取测试数据函数    
def extract_test_data(files_dir):
    #获得测试数据
    filenames = [os.path.join(files_dir, 'test.bin')]
    return extract_data(filenames)
  

#把稠密数据label[1,5...]变为[[0,1,0,0...],[...]...]
def dense_to_one_hot(labels_dense, num_classes):
    #数据数量,np.shape[0]返回行数，对于一维数据返回的是元素个数,
    #如果读取了5个文件的所有训练数据，那么现在的num_labels的值应该是50000
    num_labels = labels_dense.shape[0]
    print(num_labels)

    #生成[0,1,2...]*10,[0,10,20...],之所以这样子是每隔10个数定义一次值，
    #比如第0个，第11个，第22个......的值都赋值为1
    #index_offset = np.arange(num_labels) * num_classes

    #初始化np的二维数组,一个全0，shape为(50000,10)的数组
    labels_one_hot = np.reshape(labels_dense, [num_labels, 1])

    #相对应位置赋值变为[[0,1,0,0...],[...]...],np.flat将labels_one_hot砸平为1维
    #labels_one_hot.flat[index_offset + labels_dense.ravel()] = 1

    #print("label one hot sample = ", labels_one_hot)
    return labels_one_hot


#定义dataset数据集类
class myDataSet(object):
    
    """docstring for dataset"""
    def __init__(self, data_dir):
        super(myDataSet, self).__init__()
        self.train_labels, self.train_vectors = extract_train_data(os.path.join(data_dir, 'data'))
        self.test_labels, self.test_vectors = extract_test_data(os.path.join(data_dir, 'data'))
        
        print(self.train_labels.size)

        self.train_labels = dense_to_one_hot(self.train_labels, 1)
        self.test_labels = dense_to_one_hot(self.test_labels, 1)

        #epoch完成次数
        self.epochs_completed = 0
        #当前批次在epoch中进行的进度
        self.index_in_epoch = 0


    def next_train_batch(self, batch_size):
        #起始位置
        start = self.index_in_epoch
        self.index_in_epoch += batch_size
        #print "self.index_in_epoch: ",self.index_in_epoch
        #完成了一次epoch
        if self.index_in_epoch > TRAIN_NUMS:
            #epoch完成次数加1,1000全部训练完一次，那么没有数据用了怎么办，采取的办法就是将原来的数据集打乱顺序再用
            self.epochs_completed += 1
            #print "self.epochs_completed: ",self.epochs_completed
            #打乱数据顺序，随机性
            perm = np.arange(TRAIN_NUMS)
            np.random.shuffle(perm)
            self.train_vectors = self.train_vectors[perm]
            self.train_labels = self.train_labels[perm]
            start = 0
            self.index_in_epoch = batch_size
            #条件不成立会报错
            assert batch_size <= TRAIN_NUMS

        end = self.index_in_epoch
        #print "start,end: ",start,end

        return self.train_vectors[start:end], self.train_labels[start:end]

    def test_data(self, batch_size):     

        #打乱数据顺序，随机性
        tperm = np.arange(TEST_NUM)
        np.random.shuffle(tperm)
        self.test_vectors = self.test_vectors[tperm]
        self.test_labels = self.test_labels[tperm]
        start = 0
        self.tindex_in_epoch = batch_size
        end = self.tindex_in_epoch
        
        return self.test_vectors[start:end], self.test_labels[start:end]


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

def main():
    
    #get data
    mydata = myDataSet(os.getcwd())
    
    #reshape input to 4-d structured tensor, where 14 stand for channel
    x = tf.placeholder(tf.float32, [None, 1260])
    y_ = tf.placeholder(tf.float32, [None, 1])
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
    '''
    if y_conv. > 1000:
        y_conv = 1000
    if y_conv < -1000:
        y_conv = -1000
    '''
    #################### defining training hyperparameters ####################
    
    abserr = tf.reduce_mean(tf.abs(y_ - y_conv))
    reg_loss = regularized_loss(abserr)
    train_step = tf.train.AdamOptimizer(1e-6).minimize(reg_loss)
    
    correct_prediction = tf.abs(y_conv - y_)
    loss = tf.reduce_mean(correct_prediction)
    
    sess = tf.InteractiveSession()
    saver = tf.train.Saver() 
    tf.global_variables_initializer().run()
    
    #restore model
    saver.restore(sess, model_path)
    print('restored')
    
    avg_tr = 0
    avg_te = 0
    
    #################### training session ####################
    
    for i in range(600000):
        #train
        batch = mydata.next_train_batch(50)
        train_step.run(feed_dict = {x:batch[0], y_:batch[1], keep_prob:0.5})
        #print(sess.run(y_, feed_dict = {x:batch[0], y_:batch[1], keep_prob:0.5}))
        
        #test
        if i % 30 == 0:   
            train_accuracy = loss.eval(feed_dict = {x:batch[0], y_:batch[1], keep_prob:1.0})
            avg_tr = discounted_average_loss(i / 30, avg_tr, train_accuracy)
            batch = mydata.test_data(50)
            test_accuracy = loss.eval(feed_dict = {x:batch[0], y_:batch[1], keep_prob:1.0})
            avg_te = discounted_average_loss(i / 30, avg_te, test_accuracy)           
            print("%d: training %g; test %g" % (i, avg_tr, avg_te)) 
            
        #save model
        if(i % 1000 == 600):
            saver.save(sess, model_path)
            print('saved')
        
        #sleep
        time.sleep(0.15)
        if(i % 1000 == 800):
            print('sleeping')
            for i in range (10):
                #print('.', end = '')
                time.sleep(39)

    #################### session for test ####################
    
    batch = mydata.test_data(1)
    
    ##### data #####
    
    for i in range (500):
        print('y_conv = ')
        print(sess.run(y_conv[i], feed_dict = {x:batch[0], y_:batch[1], keep_prob:1.0}))
        print('y_ = ')
        print(sess.run(y_[i], feed_dict = {x:batch[0], y_:batch[1], keep_prob:1.0}))
        print(' ')
        time.sleep(2)
    
    ##### params #####
    
    #output layer
    print(sess.run(W, feed_dict = {x:batch[0], y_:batch[1], keep_prob:1.0}))
    print(sess.run(b, feed_dict = {x:batch[0], y_:batch[1], keep_prob:1.0}))
    
    #fc layer
    print(sess.run(W_fc, feed_dict = {x:batch[0], y_:batch[1], keep_prob:1.0}))
    print(sess.run(b_fc, feed_dict = {x:batch[0], y_:batch[1], keep_prob:1.0}))
    
    #primary conv layers
    print(sess.run(W_conv5, feed_dict = {x:batch[0], y_:batch[1], keep_prob:1.0}))
    print(sess.run(b_conv5, feed_dict = {x:batch[0], y_:batch[1], keep_prob:1.0}))
    
    #primary conv layers, with spacial structure
    '''
    a = tf.placeholder(tf.float32, [1,9,14,48])
    a = tf.reshape(W_conv5, [1,9,14,48])
    for k in range (48):
        print('conv layer ', k)
        for i in range (9):
            for j in range (14):
                print("%8.5f" %(sess.run(a[0][i][j][k], feed_dict = {x:batch[0], y_:batch[1], keep_prob:1.0})), end = ' ')
            print(' ')
        print(' ')
    '''
    #################### feel free to email me @ zhaotianyang@pku.edu.cn ####################
        
if __name__ == '__main__':
    main()
