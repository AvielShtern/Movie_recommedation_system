/**
 * @file RecommenderSystem.cpp
 * @author Aviel Shtern <aviel.shtern@mail.huji.ac.il>
 * @version 1.0
 * @date 27 jun 2020
 * @brief Movie Recommendation System for Cinema City
 */

#include "RecommenderSystem.h"

/**
 * @def PROBLEM_WITH_INPUT_FILE 1
 * @brief In case there is a problem with the file we will return 1
 */
#define PROBLEM_WITH_INPUT_FILE 1

/**
 * @def SUCCESS 0
 * @brief In case we were able to load the information correctly we would return 0
 */
#define SUCCESS 0

/**
 * @def NO_RATING "NA"
 * @brief Indicates in the input file a movie that the user did not rate
 */
#define NO_RATING "NA"

/**
 * @def MSG_ERROR_NOT_FOUND "USER NOT FOUND"
 * @brief In case the user does not exist we will return this string
 */
#define MSG_ERROR_NOT_FOUND "USER NOT FOUND"

/**
 * @def MSG_ERROR_INPUT_FILE "Unable to open file "
 * @brief In case there is a problem with the file we will concatenate the path to this message
 */
#define MSG_ERROR_INPUT_FILE "Unable to open file "

/**
 * @def INITIAL_VALUE 0.0
 * @brief initial value (for double)
 */
#define INITIAL_VALUE 0.0

/**
 * @def  INIT_VALUE  0
 * @brief initial value (for int)
 */
#define INIT_VALUE  0

/**
 * @def NOT_MOVIE "NOT_MOVIE"
 * @brief Indicates that the vector does not belong to the movie (preference vector) so we will
 * not keep its norm
 */
#define NOT_MOVIE "NOT_MOVIE"

/**
 * @def INIT_SIMILIER -1.1
 * @brief Since the similarity values ​​are between -1 and 1,
 * we can initialize an initial value with -1.1
 */
#define INIT_SIMILIER -1.1

/**
 * @def  NOT_FOUND -1.0
 * @brief Return value in case and user or movie does not exist (double)
 */
#define NOT_FOUND -1.0


/**
 * IN HEADER FILE!!
 */
int
RecommenderSystem::loadData(const string &moviesAttributesFilePath, const string &userRanksFilePath)
{
	if (loadDataOfCharacteristics(moviesAttributesFilePath) == PROBLEM_WITH_INPUT_FILE)
	{
		cout << MSG_ERROR_INPUT_FILE << moviesAttributesFilePath << endl;
		return PROBLEM_WITH_INPUT_FILE;
	}
	else if (loadDataOfUserRating(userRanksFilePath) == PROBLEM_WITH_INPUT_FILE)
	{
		cout << MSG_ERROR_INPUT_FILE << moviesAttributesFilePath << endl;
		return PROBLEM_WITH_INPUT_FILE;
	}
	return SUCCESS;

}

/**
 * IN HEADER FILE!!
 */
int RecommenderSystem::loadDataOfCharacteristics(const string &moviesAttributesFilePath)
{
	ifstream infileCharacteristics;
	infileCharacteristics.open(moviesAttributesFilePath);
	if (!infileCharacteristics)
	{
		return PROBLEM_WITH_INPUT_FILE;
	}
	_numOfMovies = INIT_VALUE;
	_numOfCharacteristics = INIT_VALUE;
	for (string line; getline(infileCharacteristics, line); )
	{
		_numOfMovies++;
		string movieName, numInStr;
		double numInInt;
		stringstream splitLine(line);
		if (splitLine >> movieName)
		{
			while (splitLine >> numInStr)
			{
				_numOfCharacteristics++;
				stringstream castNumFromStr(numInStr);
				castNumFromStr >> numInInt;
				_dataCharacteristics[movieName].push_back(numInInt);
			}
		}
	}

	_numOfCharacteristics /= _numOfMovies; /// we assume that num of chracteristics equal in all
	/// movies
	infileCharacteristics.close();
	return SUCCESS;
}

/**
 * IN HEADER FILE!!
 */
int RecommenderSystem::loadDataOfUserRating(const string &userRanksFilePath)
{
	ifstream infileUserRating;
	infileUserRating.open(userRanksFilePath);
	if (!infileUserRating)
	{
		return PROBLEM_WITH_INPUT_FILE;
	}
	string lineOfMovieNames, nameOfMovie;
	getline(infileUserRating, lineOfMovieNames);
	stringstream splitLineOfMovieNames(lineOfMovieNames);
	{
		for (size_t index = INIT_VALUE; splitLineOfMovieNames >> nameOfMovie && index < _numOfMovies;
			 index++)
		{
			_movieNamesByOrder[index] = nameOfMovie;
		}
	}
	for (string userLine; getline(infileUserRating, userLine); )
	{
		string userName, numInStr;
		double numInInt;
		stringstream splitUserLine(userLine);
		if (splitUserLine >> userName)
		{
			for (size_t numOfCol = INIT_VALUE; splitUserLine >> numInStr && numOfCol < _numOfMovies;
				 numOfCol++)
			{
				if (numInStr != NO_RATING) // the string "NA"
				{
					stringstream castNumFromStr(numInStr);
					castNumFromStr >> numInInt;
					_userRating[userName][_movieNamesByOrder[numOfCol]] = numInInt;
				}
			}
		}
	}
	infileUserRating.close();
	return SUCCESS;
}

/**
 * IN HEADER FILE!!
 */
string RecommenderSystem::recommendByContent(const string &userName)
{
	if (_userRating.find(userName) == _userRating.end())
	{
		string msg = MSG_ERROR_NOT_FOUND;
		return msg;
	}

	double average = accumulate(std::begin(_userRating[userName]), std::end(_userRating[userName])
			, INITIAL_VALUE, [](double value, const unorderMapOfStrAndDouble::value_type& p)
			{return value + p.second; } ) / _userRating[userName].size(); /// Using a STL schema
			/// algorithm. Then divide by the number of films rated by the user

	vectorOfDouble vecPreferences(_numOfCharacteristics, INITIAL_VALUE);
	for (const auto &rating: _userRating[userName])
	{
		for (size_t i = INIT_VALUE; i < _numOfCharacteristics; i++)
		{
			vecPreferences[i] += (rating.second - average) * _dataCharacteristics[rating.first][i];
		}
	}
	string winMovie;
	double max = INIT_SIMILIER;
	for (size_t i = INIT_VALUE; i < _numOfMovies; i++)
	{
		string curMovie = _movieNamesByOrder[i];
		if (_userRating[userName].find(curMovie) == _userRating[userName].end()) ///the user notsee this movie
        {
			double curent = getSimilierValue(vecPreferences, NOT_MOVIE,
											 _dataCharacteristics[curMovie],
											 curMovie);
			if (curent > max)
			{
				winMovie = curMovie;
				max = curent;
			}
        }
	}
	return winMovie;
}

/**
 * IN HEADER FILE!!
 */
double
RecommenderSystem::predictMovieScoreForUser(const string& movieName, const string& userName,
		                                    size_t k)
{
	if (_dataCharacteristics.find(movieName) == _dataCharacteristics.end() || _userRating.find(userName) == _userRating.end())
	{
		return NOT_FOUND;
	}
	multiMapOfDoubleAndStr setK;
	for (const auto & it : _userRating[userName])
	{
		double similier = getSimilierValue(_dataCharacteristics[movieName], movieName,
				_dataCharacteristics[it.first], it.first);
		setK.insert(make_pair(similier, it.first));
	}
	size_t count = k;
	double mone = INITIAL_VALUE;
	double mechane = INITIAL_VALUE;
	for (auto it = setK.rbegin() ; it != setK.rend() && count > 0; it++, count--)
	{
		mone += it->first * _userRating[userName][it->second];
		mechane += it->first;
	}
	double res = mone / mechane;
	return res;
}

/**
 * IN HEADER FILE!!
 */
string RecommenderSystem::recommendByCF(const string& userName, size_t k)
{
	if (_userRating.find(userName) == _userRating.end())
	{
		return MSG_ERROR_NOT_FOUND;
	}

	double curSum = INITIAL_VALUE;
	string currentMovieReturn;
	for (size_t i = INIT_VALUE; i < _numOfMovies; i++)
	{
		string curMovie = _movieNamesByOrder[i];
		if (_userRating[userName].find(curMovie) == _userRating[userName].end()) // the
		{

			double similier = predictMovieScoreForUser(curMovie, userName, k);
			if (similier > curSum)
			{
				curSum = similier;
				currentMovieReturn = curMovie;
			}

		}
	}
	return currentMovieReturn;
}

/**
 * IN HEADER FILE!!
 */
double
RecommenderSystem::_dotProduct(const vectorOfDouble &vec1, const vectorOfDouble &vec2, size_t n)
{
	double sum = INITIAL_VALUE;
	for (size_t i = INIT_VALUE; i < n; i++)
	{
		sum += (vec1[i] * vec2[i]);
	}
	return sum;
}

/**
 * IN HEADER FILE!!
 */
double
RecommenderSystem::getSimilierValue(const vectorOfDouble &vec1, const string &movie1
		, const vectorOfDouble &vec2, const string &movie2)
{
	size_t n = _numOfCharacteristics;
	double dotProductScalar = _dotProduct(vec1, vec2, n);
	double multuplyNorms = getNormAndKeepIfNotExit(vec1, movie1) * getNormAndKeepIfNotExit(vec2,
			movie2);
	double similier = dotProductScalar / multuplyNorms;
	return similier;
}

/**
 * IN HEADER FILE!!
 */
double RecommenderSystem::getNormAndKeepIfNotExit(const vectorOfDouble& vec, const string & movieName)
{
	double norm;
	if (_keepsNorms.find(movieName) != _keepsNorms.end())
	{
		norm = _keepsNorms[movieName];
	}
	else
	{
		double normInSquered = _dotProduct(vec, vec, _numOfCharacteristics);
		norm = sqrt(normInSquered);
		if (movieName != NOT_MOVIE)
		{
			_keepsNorms[movieName] = norm;
		}
	}
	return norm;
}