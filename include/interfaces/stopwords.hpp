#ifndef TEXTSIM_STOPWORDS_HPP
#define TEXTSIM_STOPWORDS_HPP
namespace textsim{
class stopwords{
 public:
  virtual bool is_stopword(std::string input) = 0;
};
}

#endif //TEXTSIM_STOPWORDS_HPP
