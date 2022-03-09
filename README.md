# Unsupervised Noise Classification on Smartphones


## Overview
This GitHub repository provides for Real-Time Smartphone App for Unsupervised Noise Classification in Realistic Audio Environments.

> **Abstract:** This paper presents a real-time unsupervised noise classifier smartphone app which is designed to operate in realistic audio environments. This app addresses the two limitations of a previously developed smartphone app for unsupervised noise classification. A voice activity detection is added to separate the presence of speech frames from noise frames and thus to lower misclassifications when operating in realistic audio environments. In addition, buffers are added to allow a stable operation of the noise classifier in the field. The unsupervised noise classification is achieved by fusing the decisions of two adaptive resonance theory unsupervised classifiers running in parallel. One classifier operates on subband features and the other operates on mel- frequency spectral coefficients. The results of field testing indicate the effectiveness of this unsupervised noise classifier app when used in realistic audio environments.

You can find the paper for this GitHub repository: https://ieeexplore.ieee.org/abstract/document/8662052

## Audio-Video Demo

- https://labs.utdallas.edu/ssprl/files/2020/09/17Unsupervised-Classifier-Video1.mp4

## Users Guides

- [User Guide](Users-Guide-UnsupervisedNoiseClassification)

## Requirement
- Tensorflow C++ API
- Superpowered


## License and Citation
The codes are licensed under MIT license.

For any utilization of the code content of this repository, one of the following books needs to get cited by the user:

- N. Alamdari and N. Kehtarnavaz, “A Real-Time Smartphone App for Unsupervised Noise Classification in Realistic Audio Environments,” Proceedings of IEEE International Conference on Consumer Electronics, Las Vegas, NV, Jan 2019.

## Disclaimer
This work was supported in part by the National Institute of the Deafness and Other Communication Disorders (NIDCD) of the National

Institutes of Health (NIH) under Award 1R01DC015430. The content is solely the responsibility of the authors and does not necessarily represent the official views of the NIH
