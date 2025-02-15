### Development Status

| Image | Meaning |
| ----- |---------|
|  🟢️   | Done |
|  🔴️   | Todo |
|  ⚫️   | Not planned / Not supported |

# Layers
---

## Core layers

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| ----| ------|---------|
| Dense     | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Just your regular densely-connected NN layer |
| Dropout   | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Applies Dropout to the input |
| Flatten   | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Flattens the input. Does not affect the batch size. (Wrapper for Reshape) |
| Input     | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Used to instantiate a EDDL tensor |
| Reshape   | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Returns a new layer with the same data and number of elements as input, but with the specified shape |
| Squeeze   | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️ | Returns a new layer with all the dimensions of input of size 1 removed |
| Unsqueeze | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️ | Returns a new layer with a dimension of size one inserted at the specified position |
| Select        | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️ | Returns a new layer which indexes the input tensor using the entries in indices |
| Slice        | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️ | Returns a new layer which indexes the input tensor using the entries in indices. (alias for Select) |
| Permute   | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️  | Permutes the dimensions of the input according to a given pattern |
| Split        | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️ | Split a layer into a list of tensors layers |
| Embedding | 🟢️️ | 🟢️️ | ⚫️️ | ️🟢️️ | Turns positive integers (indexes) into dense vectors of fixed size; (also known as mapping). e.g. `[[4], [20]] -> [[0.25, 0.1], [0.6, -0.2]]` |
| Transpose | 🟢️️ | 🟢️️ | ⚫️️ | ️🟢️️ | Permute the last two dimensions |
| ConstOfTensor | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️ | Repeats a tensor across the batch |
| Expand        | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️ | Returns a layer with singleton dimensions expanded to a larger size |
| Where | 🟢️️ | 🟢️️ | ⚫️️ | ️🔴️ | Return elements chosen from x or y depending on condition |
| Resize          | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️ | Resize the input image to the given size. `[height, width]` |
| Clamp / Clip          | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️ | Clamps all elements in input into the range `[min, max]`.  |
| Repeat          | 🟢️️ | 🟢️️ | ⚫️️ | 🔴️️ | Repeats the elements of a tensor along the specified dimension. (Elements in an axis can be repeated independently)  |
| Tile          | 🟢️️ | 🟢️️ | ⚫️️ | 🔴️️ | Repeats the elements of a tensor along the specified dimensions.  |
| Broadcast          | 🟢️️ | 🟢️️ | ⚫️️ | 🔴️️ | Produce an object that mimics broadcasting. |
| Shape          | 🟢️️ | 🟢️️ | ⚫️️ | 🔴️️ | Returns the shape of its parent as his output |


## Activations

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| -----| ------|---------|
| ELU           | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Exponential linear unit |
| Exponential   | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ (Custom Op) | Exponential (base e) activation function |
| HardSigmoid   | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Hard sigmoid activation function |
| LeakyReLu     | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Leaky version of a Rectified Linear Unit |
| Linear        | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ (Custom Op) | Linear (i.e. identity) activation function |
| ReLu          | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Rectified Linear Unit |
| Softmax       | 🟢️️ | 🟢️️ | 🟢️ | 🟢️️ | Softmax activation function |
| Selu          | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Scaled Exponential Linear Unit (SELU) |
| Sigmoid       | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Sigmoid activation function |
| Softplus      | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Softplus activation function |
| Softsign      | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Softsign activation function |
| Tanh          | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Hyperbolic tangent activation function |
| ThresholdedReLU | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Thresholded Rectified Linear Unit |
| PReLU         | ⚫️️ | ⚫️ | ⚫️ | ⚫️️ | Parametric Rectified Linear Unit |


## Convolutional layers

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| -----| ------|---------|
| Conv1D            | 🟢️️ | 🟢️️ | 🟢️ | 🟢️️ | 1D convolution |
| Conv2D            | 🟢️️ | 🟢️️ | 🟢️ | 🟢️️ | 2D convolution |
| Conv3D            | ⚫️️ | ⚫️ | 🟢️️ | 🟢️ | 3D convolution |
| Pointwise         | 🟢️️ | 🟢️️ | 🟢️ | 🟢️️ | 2D pointwise convolution |
| DepthwiseConv2D   | ⚫️ | ⚫ | 🟢️ | 🟢️ | 2D depthsise convolution |
| TransposedConv2D  | ⚫️️ | ⚫️ | 🟢️ | 🟢️ | 2D Transposed convolution |
| TransposedConv3D  | ⚫️️ | ⚫️ | 🟢️ | 🟢️ | 3D Transposed convolution |
| UpSampling2D      | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Practically the same as `Scale(mode="nearest")`. Instead of performing nearest interpolation, this works by repeating n times the elements of each axis `[2, 1] => [2, 2, 1, 1]` |
| UpSampling3D        | 🟢️️ | 🟢️️ | ⚫️️ | ️️🟢️️ | Practically the same as `Scale(mode="nearest")`. Instead of performing nearest interpolation, this works by repeating n times the elements of each axis `[2, 1] => [2, 2, 1, 1]` |


## Pooling layers

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| -----| ------|---------|
| MaxPool1D           | 🟢️️ | 🟢️️ | 🟢️️️ | 🟢️️ | 1D MaxPooling operation |
| MaxPool2D           | 🟢️️ | 🟢️️ | 🟢️️ | 🟢️️ | 2D MaxPooling operation |
| MaxPool3D           | 🟢️️ | 🟢️ | 🟢️️️ | 🟢️ | 3D MaxPooling operation |
| AveragePool1D       | 🟢️️ | 🟢️️ | 🟢️️️ | 🟢️️ | 1D AveragePooling operation |
| AveragePool2D       | 🟢️️ | 🟢️️ | 🟢️️️ | 🟢️️ | 2D AveragePooling operation |
| AveragePool3D       | 🟢️️ | 🟢️ | 🟢️️️️ | 🟢️️️ | 3D AveragePooling operation |
| GlobalMaxPool1D     | 🟢️️ | 🟢️️ | 🟢️️️ | 🟢️️ | 1D GlobalMaxPooling operation |
| GlobalMaxPool2D     | 🟢️️ | 🟢️️ | 🟢️️️ | 🟢️️ | 2D GlobalMaxPooling operation |
| GlobalMaxPool3D     | 🟢️️ | 🟢️ | 🟢️️️ | 🟢️️️ | 3D GlobalMaxPooling operation |
| GlobalAveragePool1D | 🟢️️ | 🟢️️ | 🟢️️️ | 🟢️️ | 1D GlobalAveragePooling operation |
| GlobalAveragePool2D | 🟢️️ | 🟢️️ | 🟢️️️ | 🟢️️ | 2D GlobalAveragePooling operation |
| GlobalAveragePool3D | 🟢️️ | 🟢️ | 🟢️️️️ | 🟢️️️ | 3D GlobalAveragePooling operation |


## Data transformation/augmentation

### Data transformations

Deterministic transformations

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| -----| ------|---------|
| Crop           | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️️ | Crops the given image at `[(top, left), (bottom, right)]` |
| CenteredCrop   | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Crops the given image at the center with size (width, height)  |
| CropScale      | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Crop the given image at `[(top, left), (bottom, right)]` and scale it to the parent size |
| Cutout         | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Selects a rectangle region in an image at `[(top, left), (bottom, right)]` and erases its pixels using a constant value |
| Flip           | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Flip the given image at `axis=n` |
| HorizontalFlip | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Horizontally flip the given image |
| Pad            | 🟢️️ | 🟢️️ | ⚫ | 🟢️ | Pad the given image on all sides with the given "pad" value |
| Rotate         | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Rotate the image by angle |
| Scale          | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Resize the input image to the given size. `[height, width]`. Does not include backward (see Resize) |
| Shift          | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Shift the input image `[a, b]` |
| VerticallyFlip | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Vertically flip the given image |


### Data augmentations

Apply data transformations with random parametrization.

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| -----| ------|---------|
| RandomCrop           | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Crop the given image at a random location with size `[height, width]`  |
| RandomCropScale      | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Crop the given image randomly by the size in a range `[a, b]` by and scale it to the parent size |
| RandomCutout         | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Randomly selects a rectangle region in an image and erases its pixels. The random region is defined by the range `[(min_x, max_x), (min_y, max_y)]`, where these are relative values |
| RandomFlip           | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Flip the given image at `axis=n` randomly with a given probability |
| RandomHorizontalFlip | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Horizontally flip the given image randomly with a given probability |
| RandomRotation       | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Rotate the image randomly by an angle defined in a range `[a, b]` |
| RandomScale          | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Resize the input image randomly by the size in a range `[a, b]` |
| RandomShift          | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Shift the input image randomly in range `[a, b]` |
| RandomVerticalFlip   | 🟢️️ | 🟢️️ | ⚫️️ | ⚫️ | Vertically flip the given image randomly with a given probability |


## Merge layers

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| -----| ------|---------|
| Add           | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️  | Layer that adds a list of inputs |
| Concatenate   | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Layer that concatenates a list of inputs |
| Average       | ⚫ | ⚫️ | ⚫️ | ⚫ | Layer that averages a list of inputs |
| Dot           | ⚫ | ⚫️ | ⚫️ | ⚫ | Layer that computes a dot product between samples in two tensors |
| Multiply      | ⚫ | ⚫️ | ⚫️ | ⚫ | Layer that multiplies (element-wise) a list of inputs |
| Maximum       | ⚫ | ⚫️ | ⚫️ | ⚫ | Layer that computes the maximum (element-wise) a list of inputs |
| Minimum       | ⚫ | ⚫️ | ⚫️ | ⚫ | Layer that computes the minimum (element-wise) a list of inputs |
| Substract     | ⚫ | ⚫️ | ⚫️ | ⚫ | Layer that subtracts two inputs |


## Normalization

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| -----| ------|---------|
| BatchNormalization | 🟢️️ | 🟢️️ | 🟢️ | 🟢️️ | Batch normalization layer (Ioffe and Szegedy, 2014) |
| LayerNormalization | 🟢️️ | 🟢️️ | ⚫️️ | ⚫ (Not in ONNX) | Layer normalization layer (Ba et al., 2016)  |
| GroupNormalization | 🟢️️ | 🟢️️ | ⚫️️ | ⚫ (Not in ONNX) | Group normalization layer (Yuxin Wu and Kaiming He, 2018) |
| Norm               | 🟢️️ | 🟢️️ | ⚫️️ | ⚫ (Not in ONNX) |   |
| NormMax            | 🟢️️ | 🟢️️ | ⚫️️ | ⚫ (Not in ONNX) |   |
| NormMinMax         | 🟢️️ | 🟢️️ | ⚫️️ | ⚫ (Not in ONNX) |   |


## Noise layers

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| -----| ------|---------|
| GaussianNoise | 🟢️️ | 🟢️️ | ⚫️️ | ⚫ (Not in ONNX) | Apply additive zero-centered Gaussian noise |
| UniformNoise  | 🟢️️ | 🟢️️ | ⚫️️ | ⚫ (Not in ONNX) | Apply additive zero-centered uniform noise |


## Operators layers

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| -----| ------|---------|
| Abs           |  🟢️️| 🟢️️ | ⚫️️️️ | 🟢️️ | |
| Sum           | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | |
| Div           | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | |
| Exp           | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | |
| Log           | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | |
| Log2          |  🟢️️| 🟢️️ |⚫️️ | ⚫ (Not in ONNX) | |
| Log10         | 🟢️️ | 🟢️️ | ⚫️️ | ⚫ (Not in ONNX) | |
| Mult          | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | |
| Pow           | 🟢️️ | 🟢️️ | ⚫️️ | 🟢 | |
| Sqrt          |  🟢️️| 🟢️️ |⚫️️️ | 🟢️️ | |
| Sub           | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | |
| Round          | 🔴️️ | 🔴️️ | ⚫️️ | 🔴️️ | Round of the elements of input |
| Ceil          | 🔴️ | 🔴️️ | ⚫️️ | 🔴️️ | Ceil of the elements of input  |
| Floor          | 🔴️️ | 🔴 |⚫️️ | 🔴️️ | Floor of the elements of input  |
| Equal | 🟢️️ | 🟢️️ | ⚫️️ | ️🔴️ | Return (x1 == x2) element-wise |

## Reduction layers

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| -----| ------|---------|
| Max    | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | |
| Mean   | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | |
| Min    | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | |
| Sum    | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | |
| Var    | 🟢️️ | 🟢️️ | ⚫️️ | ⚫ (Not in ONNX) | |
| Argmax | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | |


## Reurrent layers

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| -----| ------|---------|
| GRU  | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️ | Gated Recurrent Unit - Cho et al. 2014 |
| LSTM | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️️ | Long Short-Term Memory layer - Hochreiter 1997 |
| RNN  | 🟢️️ | 🟢️️ | ⚫️️ | 🟢️ | Fully-connected RNN where the output is to be fed back to input |


## Regularizer layers

| Functionality | CPU | GPU | cuDNN | ONNX | Comments |
| ------------- |------| -----| -----| ------|---------|
| L1   | 🟢️️ | 🟢️️ | ⚫️️ | ⚫ | Lasso Regression |
| L2   | 🟢️️ | 🟢️️ | ⚫️️ | ⚫ | Ridge Regression |
| L1L2 | 🟢️️ | 🟢️️ | ⚫️️ | ⚫ | Lasso Regression + Ridge Regression |


# Initializers

| Functionality | CPU | GPU | cuDNN | Comments |
| ------------- |------| -----| ------| ---------|
| Constant        | 🟢️️ | 🟢️️ | ⚫️️ | Initializer that generates tensors initialized to a constant value |
| GlorotNormal    | 🟢️️ | 🟢️️ | ⚫️️ | Glorot normal initializer, also called Xavier normal initializer |
| GlorotUniform   | 🟢️️ | 🟢️️ | ⚫️️ | Glorot uniform initializer, also called Xavier uniform initializer |
| HeNormal        | 🟢️️ | 🟢️️ | ⚫️️ | _He_ normal initializer |
| HeUniform       | 🟢️️ | 🟢️️ | ⚫️️️️ | _He_ uniform initializer |
| RandomNormal    | 🟢️️ | 🟢️️ | ⚫️️ | Initializer that generates tensors with a normal distribution |
| RandomUniform   | 🟢️️ | 🟢️️ | ⚫️️ | Initializer that generates tensors with a uniform distribution |
| Identity        | ⚫️ | ⚫️ | ⚫️ | Initializer that generates the identity matrix |
| LeCunUniform    | ⚫ | ⚫ | ⚫ | LeCun uniform initializer |
| LeCunNormal     | ⚫ | ⚫ | ⚫ | LeCun normal initializer |
| Orthogonal      | ⚫️ | ⚫ | ⚫ | Initializer that generates a random orthogonal matrix |
| TruncatedNormal | ⚫ | ⚫ | ⚫ | Initializer that generates a truncated normal distribution |
| VarianceScaling | ⚫ | ⚫️ | ⚫️ | Initializer capable of adapting its scale to the shape of weights |


# Constraints

| Functionality | CPU | GPU | cuDNN | Comments |
| ------------- |------| -----| ------| ---------|
| MaxNorm    | ⚫️  | ⚫️️ | ⚫️️ | MaxNorm weight constraint |
| MinMaxNorm | ⚫️  | ⚫️️ | ⚫️️ | MinMaxNorm weight constraint |
| NonNeg     | ⚫️  | ⚫️️ | ⚫️️ | Constrains the weights to be non-negative |
| UnitNorm   | ⚫️  | ⚫️️ | ⚫️️ | Constrains the weights incident to each hidden unit to have unit norm |


# Loss functions

| Functionality | CPU | GPU | cuDNN | Comments |
| ------------- |------| -----| ------| ---------|
| CategoricalCrossEntropy | 🟢️️ | 🟢️️ | ⚫️️ | CCE (The output is represented by n values that represent the probabilities each class) |
| BinaryCrossEntropy      | 🟢️️ | 🟢️️ | ⚫️️ | BCE (The output is represented by a single value that represent the probability of the second class) |
| MSE                     | 🟢️️ | 🟢️️ | ⚫️️ | Mean Squared Error |
| Min                     | 🟢️️ | 🟢️️ | ⚫️️ | Minimum Error |
| Dice                    | 🟢️️ | 🟢️️ | ⚫️️ | Dice loss |
| SoftCrossEntropy        | 🟢️️ | 🟢️️ | ⚫️️ | Soft-Categorical Cross-Entropy Error |
| MAE                     | ⚫ | ⚫ | ⚫ | Mean Absolute Error  |
| MRE                     | ⚫ | ⚫ | ⚫ | Mean Relative Error |
| MSLE                    | ⚫ | ⚫ | ⚫ | Mean Squared Logarithmic Error |
| Hinge                   | ⚫ | ⚫ | ⚫ | Hinge Error |


# Metric functions

| Functionality | CPU | GPU | cuDNN | Comments |
| ------------- |------| -----| ------| ---------|
| CategoricalAccuracy | 🟢️️ | 🟢️️ | ⚫️️ | |
| MSE                 | 🟢️️ | 🟢️️ | ⚫️️ | Mean Squared Error |
| MAE                 | 🟢️️ | 🟢️️ | ⚫️️ | Mean Absolute Error  |
| MRE                 | 🟢️️ | 🟢️️ | ⚫️️ | Mean Relative Error |
| Sum                 | 🟢️️ | 🟢️️ | ⚫️️ | Sum Error |
| Dice                | 🟢️️ | 🟢️️ | ⚫️️ | Dice error |
| TopKAccuracy        | ⚫ | ⚫ | ⚫ | |
| CosineProximity     | ⚫ | ⚫ | ⚫ | |


# Optimizers

| Functionality | CPU | GPU | cuDNN | Comments |
| ------------- |------| -----| ------| ---------|
| Adam     | 🟢️️ | 🟢️️ | ⚫️️ | Adam optimizer |
| RMSProp  | 🟢️️ | 🟢️️ | ⚫️️ | RMSProp optimizer |
| SGD      | 🟢️️ | 🟢️️ | ⚫️️ | Stochastic gradient descent optimizer |
| Adadelta |⚫ |⚫ | ⚫ | Adadelta optimizer |
| Adagrad  |⚫ |⚫ | ⚫ | Adagrad optimizer |
| Adamax   |⚫ |⚫ | ⚫ | Adamax optimizer from Adam paper's Section 7 |
| Nadam    |⚫ |⚫ | ⚫ | Nesterov Adam optimizer |

