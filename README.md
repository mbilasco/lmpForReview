# Dataset configurations #

Dataset configurations used for experiments were attentively stored in order to be able to reproduce the experiments with the same data settings.

Various CSV files (about CASME, SMIC, CK+, MMI and Oulu-Casia) are available here: https://nextcloud.univ-lille.fr/index.php/s/XGJkjdjnqeYmk7w

Each CSV file is structured as follow :
```
expression_type;subject_number;sequence_number
```

Expression type values used for all dataset :
1. happy
2. fear
3. surprise
4. anger
5. disgust
6. sad

All dataset where restructured using the CK+ model.
```
 - subject_number
    - sequence number
      - img_001.png
      - ...
```

Dataset have to be structured as indicated earlier. Besides for, each sequence landmarks for all frames of the video need to be provided in the following format :

```
sequence_no ; x1 ; y1 ; x2 ; y2 ; ... ; x68 ; y68 # corresponding to the 1st frame of the sequence
sequence_no ; x1 ; y1 ; x2 ; y2 ; ... ; x68 ; y68 # corresponding to the 2nd frame of the sequence
sequence_no ; x1 ; y1 ; x2 ; y2 ; ... ; x68 ; y68 # corresponding to the 3rd frame of the sequence
...
```

An example is provided here : https://nextcloud.univ-lille.fr/index.php/s/PBN2dYbkGei7fGZ

The landmarks where generated using Kazemi et al. method.

# LMP #

LMP contains the source code needed to extract LMP descriptors between two frames of a video sequence.

A Makefile is provided for compiling the code.
```
make clean
make
```

To extract GMDs (accumulation of LMPs) over an entire sequence use the following command :
```
./lmpExtractor 3 0.5 0.75 100 4 5 6 12 PATH_TO_THE_DATASET EMOTION_CONFIGURATION
```

The PATH_TO_THE_DATASET value corresponds to the path of the dataset directory containing the Suject_Number/Sequence_Number folders.

The EMOTION_CONFIGURATION is a csv placed in PATH_TO_THE_DATASET and indicates the sequences used for the analysis. The file format is as follows :
```
expression_type;subject_number;sequence_number
```

The output is a GMD_*.csv file that contains a LibSVM-ready file containing for each subject/sequence a line describing the expression_type, followed by the cumulated lmp features over the entire sequence.

We use LibSVM (tools/easy.py) for conducting cross-validation protocol using 10 folds.

# DEMO #

[CK+ demo](https://nextcloud.univ-lille.fr/index.php/s/tZFKLB3r8izgaPY)

[CASIA demo](https://nextcloud.univ-lille.fr/index.php/s/dEdrNjZKgKoKjbA)

[CASME2 demo](https://nextcloud.univ-lille.fr/index.php/s/5sDc3GZ4a5awLkC)
