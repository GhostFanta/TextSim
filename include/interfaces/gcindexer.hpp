#ifndef TEXTSIM_GCINDEXER_HPP
#define TEXTSIM_GCINDEXER_HPP
namespace textsim {

class GCIndexer{
 public:
  // Get all the contexts of the given bigram string in the fourgram corpus.
  virtual std::vector <uint32_t> get_contexts(std::string targetphrase) = 0;
};

}  // namespace phsim
#endif //TEXTSIM_GCINDEXER_HPP
