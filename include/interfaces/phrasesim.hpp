#ifndef TEXTSIM_PHRASESIM_HPP
#define TEXTSIM_PHRASESIM_HPP
namespace textsim{

class Phrasesim{
 public:
  virtual double phrasesimilarity(std::string phrase1, std::string phrase2) = 0;
};
}

#endif //TEXTSIM_PHRASESIM_HPP
