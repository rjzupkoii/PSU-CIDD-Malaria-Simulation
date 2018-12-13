//
// Created by merlinvn@gmail.com on 29/12/16.
//

#ifndef MODERNCPP_SINGLEARRAYPERSONINDEX_H
#define MODERNCPP_SINGLEARRAYPERSONINDEX_H

#include "Core/Index/IObjectIndexer.h"
#include "Person.h"

template<Person::Property ... Properties>
class PersonIndexer : IObjectIndexer<std::shared_ptr<Person>, Person::Property> {
  size_t id_;
 public:
  static size_t ID;

  size_t get_id() const {
    return id_;
  }

  std::vector<Person::Property> tracking_properties{Properties...};
  using PersonNestedVector = MultidimensionalVector<std::shared_ptr<Person>, sizeof...(Properties) + 1>;
  typename PersonNestedVector::type persons;

  PersonIndexer() {
    id_ = PersonIndexer<>::ID++;
  }

  virtual ~PersonIndexer() {
    PersonIndexer<>::ID--;
  }

  //remove copy and assign operator
  PersonIndexer(const PersonIndexer &) = delete;

  PersonIndexer &operator=(const PersonIndexer &) = delete;

  PersonIndexer(const PersonIndexer &&) = delete;

  PersonIndexer &operator=(const PersonIndexer &&) = delete;

  void init(const std::initializer_list<int> &property_info) {
    if (property_info.size()!=tracking_properties.size()) {
      throw std::invalid_argument("wrong number of property info");
    }
    persons = PersonNestedVector::create(property_info.begin());
  }

  void add(std::shared_ptr<Person> &person) override {
    PersonNestedVector::add(id_, persons, tracking_properties, 0, person);
  }

  void remove(std::shared_ptr<Person> &person) override {
    PersonNestedVector::remove(id_, persons, tracking_properties, 0, person);
  }

  void size() override {}

  void update() override {}

  void notify_change(std::shared_ptr<Person> &person, const Person::Property &property, int old_value,
                     int new_value) override {}

};

template<Person::Property ... Properties>
size_t PersonIndexer<Properties...>::ID = 0;

#endif //MODERNCPP_SINGLEARRAYPERSONINDEX_H
