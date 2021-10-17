/**
 * @file RecommenderSystem.h
 * @author Aviel Shtern <aviel.shtern@mail.huji.ac.il>
 * @version 1.0
 * @date 27 jun 2020
 * @brief Movie Recommendation System for Cinema City
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <unordered_map>
#include <map>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath>


#ifndef EX5_RECOMMENDERSYSTEM_H
#define EX5_RECOMMENDERSYSTEM_H

using std::unordered_map;
using std::map;
using std::multimap;
using std::string;
using std::vector;
using std::stringstream;
using std::ifstream;
using std::cout;
using std::endl;
using std::make_pair;
using std::accumulate;
using std::begin;
using std::end;

typedef std::unordered_map<std::string, std::vector<double>> unorderMapOfStrAndVec;
typedef std::unordered_map<std::string, std::unordered_map<std::string, double>>
		unorderMapOfStrAndMapOfStrAndInt;
typedef std::unordered_map<size_t, std::string> unorderMapOfIntAndStr;
typedef std::unordered_map<std::string, double> unorderMapOfStrAndDouble;
typedef std::vector<double> vectorOfDouble;
typedef std::multimap<double, std::string> multiMapOfDoubleAndStr;

/**
 * @class RecommenderSystem
 * @brief Movies recommendation system
 */
class RecommenderSystem
{
private:
	/**
	 * @brief Contains all existing movies as keys and their preferences vector as values
	 */
	unorderMapOfStrAndVec _dataCharacteristics;
	/**
    * @brief Contains all users as keys and values are all movies they rated (!) Along with their
    * rating. A movie that the user has not rated will not appear !!.
    */
	unorderMapOfStrAndMapOfStrAndInt _userRating;
	/**
    * @brief Contains all existing movies in the order in which they appeared in the rating file.
    * The key is the index and the value is the name of the movie
    */
	unorderMapOfIntAndStr _movieNamesByOrder;
	/**
    * @brief The number of movies that exist
    */
	size_t _numOfMovies;
	/**
    * @brief The number of features that exist for each movie.
    * This is actually the size of "feature vector" and "vector preference"
    */
	size_t _numOfCharacteristics;
	/**
    * @brief If using one of the methods of the class we had to calculate a norm of vector preferences.
    * So we will keep it in ״_keepsNorms" so that if we have to recalculate simply "pull out" from here
    * Contains the name of the movie as a key and the norm as a value
    */
	unorderMapOfStrAndDouble _keepsNorms;

	/**
	 * Calculate the "Dot product״ of two vectors
	 * @param vec1
	 * @param vec2
	 * @param n size of vec ("the dim: N")
	 * @return the dot product of vec1 and vec 2 (double)
	 */
	static double _dotProduct(const vectorOfDouble &vec1, const vectorOfDouble &vec2, size_t n);


public:
	/**
	 * Loading information into the fields _dataCharacteristics and _userRating
	 * In addition: Prints an error message if there was a problem
	 * @param moviesAttributesFilePath path to file of movies attributes
	 * @param userRanksFilePath path to file of user ranks
	 * @return 0 in case of success 1 in case of failure (suppose one file does not open)
	 */
	int loadData(const string &moviesAttributesFilePath, const string &userRanksFilePath);

	/**
	 * Loading information into the field _dataCharacteristics
	 * @param moviesAttributesFilePath path to file of movies attributes
	 * @return 0 in case of success 1 in case of failure (suppose the file does not open)
	 */
	int loadDataOfCharacteristics(const string &moviesAttributesFilePath);

	/**
	 * Loading information into the field _userRating
	 * @param userRanksFilePath path to file of user ranks
	 * @return 0 in case of success 1 in case of failure (suppose the file does not open)
	 */
	int loadDataOfUserRating(const string &userRanksFilePath);

	/**
	 * Given a username we will find the movie recommended by the "Recommended by Content" algorithm
	 * @param userName The username we would like to recommend
	 * @return Recommended movie name (string). If the user does not exist in the repository we
	 * will return "USER NOT FOUND" as string.
	 */
	string recommendByContent(const string &userName);

	/**
	 * Predicting a user rating for a movie he has not seen
	 * @param movieName Name of a movie the user hasn't seen yet
	 * @param userName The username
	 * @param k The number of movies (which the user has already seen) that are most similar to movieName
	 * based on the prediction algorithm
	 * @return Predicting the ranking (double) of userName for movieName according to the
	 * "collaborative filtering - CF" algorithm
	 */
	double predictMovieScoreForUser(const string &movieName, const string &userName, size_t k);

	/**
	 * Returns the movie we would recommend to userName (By  "collaborative filtering - CF"
	 * algorithm)
	 * @param userName The username
	 * @param k The number of movies (which the user has already seen) that are most similar to movieName
	 * based on the prediction algorithm
	 * @return the movie we would recommend to userName
	 */
	string recommendByCF(const string &userName, size_t k);

	/**
	 * Calculates the similarity value of two vectors
	 * @param vec1 Vector features of the first movie
	 * @param movie1 The name of first movie
	 * @param vec2 Vector features of the second movie
	 * @param movie2 The name of second movie
	 * @return the similarity value (double)
	 */
	double getSimilierValue(const vectorOfDouble &vec1, const string &movie1
			, const vectorOfDouble &vec2, const string &movie2);

	/**
	 * Returns the norm of vector preference, In case the norm has not yet been calculated,
	 * we will consider the norm and save in _keepsNorms
	 * @param vec Vector features of movie
	 * @param movieName the name of movie
	 * @return the norm of the vector features
	 */
	double getNormAndKeepIfNotExit(const vectorOfDouble &vec, const string &movieName);
};


#endif //EX5_RECOMMENDERSYSTEM_H
