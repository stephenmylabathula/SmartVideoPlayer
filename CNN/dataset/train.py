import os
import numpy as np
import h5py

from keras.preprocessing.image import ImageDataGenerator
from keras.models import Sequential
from keras.layers import Convolution2D, MaxPooling2D, ZeroPadding2D
from keras.layers import Activation, Dropout, Flatten, Dense

train_width, train_height = 150, 113
weights_file = 'trained_weights.h5'

train_directory = 'unproc/train'
validation_directory = 'unproc/validation'

num_training_samples = 888
num_validation_samples = 300
num_epoch = 50
batch_size = 16

model = Sequential()

model.add(Convolution2D(64, 3, 3, input_shape=(3, train_width, train_height), activation='relu'))
model.add(MaxPooling2D(pool_size=(2, 2)))

model.add(Convolution2D(64, 3, 3, activation='relu'))
model.add(MaxPooling2D(pool_size=(2, 2)))

model.add(Convolution2D(128, 3, 3))
model.add(MaxPooling2D(pool_size=(2, 2)))

model.add(Flatten())
model.add(Dense(256, activation='relu'))
model.add(Dropout(0.01))
model.add(Dense(256, activation='relu'))
model.add(Dropout(0.01))
model.add(Dense(1, activation='sigmoid'))

model.compile(
	loss='binary_crossentropy',
	optimizer='sgd',
	metrics=['accuracy']
)

train_datagen = ImageDataGenerator(
	rescale=1./255,
	shear_range=20.0,
	zoom_range=20.0,
	horizontal_flip=True
)

test_datagen = ImageDataGenerator(rescale=1./255)

train_generator = train_datagen.flow_from_directory(
	train_directory,
	target_size=(train_width, train_height),
	batch_size=batch_size,
	class_mode='binary'
)

validation_generator = test_datagen.flow_from_directory(
	validation_directory,
	target_size=(train_width, train_height),
	batch_size=batch_size,
	class_mode='binary'
)

model.fit_generator(
	train_generator,
	samples_per_epoch = num_training_samples,
	nb_epoch=num_epoch,
	validation_data=validation_generator,
	nb_val_samples=num_validation_samples
)

model.save_weights(weights_file)
