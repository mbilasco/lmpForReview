HEADERS = lmp/sequencemanager.h lmp/sequenceanalyser.h lmp/faceAnalyser/faceanalyser.h lmp/faceAnalyser/ouranalyser.h lmp/faceSeparator/faceseparator.h lmp/faceSeparator/ourroi.h lmp/faceSeparator/faceroi.h lmp/filterMotion/patch.h lmp/lmp_common_header.h lmp/motionextractor.h

OBJECTS = lmp/sequencemanager.o lmp/sequenceanalyser.o lmp/faceAnalyser/faceanalyser.o lmp/faceAnalyser/ouranalyser.o lmp/faceSeparator/faceseparator.o lmp/faceSeparator/ourroi.o lmp/faceSeparator/faceroi.o lmp/filterMotion/patch.o lmp/motionextractor.o main.o

OUTPUT = lmpExtractor
INCLUDE = -I $(OPENCV_INCLUDE)
LIBS = -L $(OPENCV_LIBS) 
FLAGS= -std=gnu++11
default: $(OUTPUT)

%.o: %.cpp $(HEADERS)
	g++ $(FLAGS) -c $(INCLUDE) $< -o $@

$(OUTPUT): $(OBJECTS)
	g++ $(FLAGS) $(OBJECTS) -o $@ -lm -lstdc++ $(INCLUDE) $(LIBS)  -lopencv_core -lopencv_imgproc -lopencv_video -lopencv_highgui -lopencv_ml -lopencv_objdetect -lopencv_videoio -lopencv_videostab -lopencv_flann -lopencv_features2d -lopencv_calib3d -lopencv_imgcodecs -lopencv_tracking -lopencv_optflow -fpermissive 

clean:
	-rm -f $(OBJECTS)
	-rm -f $(OUTPUT)

