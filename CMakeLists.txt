CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)


SET(PROJECT_NAME sbp)
PROJECT(${PROJECT_NAME})

set(CMAKE_AUTOMOC ON)

SET( INCS
	src/Matrix.h
	src/Moves.h
	src/Search.h
	src/Node.h
	src/CostNode.h
)
SET( SRCS
	src/main.cpp
	src/Matrix.cpp
	src/Search.cpp
	src/Node.cpp
	src/CostNode.cpp
)

ADD_EXECUTABLE( 
	${PROJECT_NAME}
	${INCS}
	${SRCS}
)

TARGET_LINK_LIBRARIES( 
	${PROJECT_NAME} 
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
