// Copyright 2019 - present Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <gtest/gtest.h>

#include "rosidl_runtime_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_cpp/message_type_support.hpp"

// Generic MicroXRCE-DDS typesupport includes
#include <rosidl_typesupport_microxrcedds_cpp/identifier.hpp>
#include <rosidl_typesupport_microxrcedds_c/message_type_support.h>

// Specific defined types used during testing
#include "rosidl_typesupport_microxrcedds_test_msg/msg/primitive.hpp"
#include "rosidl_typesupport_microxrcedds_test_msg/msg/sequence.hpp"

/*
 * @brief TestTypeSupport class, used to automate typesupport testing for a specific type.
 */
template <typename T>
class TestTypeSupport : public ::testing::Test
{
public:
  /*
   * @brief Default constructor.
   */
  TestTypeSupport() = default;

  /*
   * @brief Lvalue copy constructor (deleted).
   * @param[in] other Test to be copied.
   */
  TestTypeSupport(
      const TestTypeSupport & other) = delete;

  /*
   * @brief Rvalue copy constructor (deleted).
   * @param[in] other Test to be copied.
   */
  TestTypeSupport(
      TestTypeSupport && other) = delete;

  /*
   * @brief Setup function.
   * @param[in] init_test_type      Initialized rvalue instance of the ROS 2 type to be tested.
   * @param[in] compare_func_handle Function used for comparing two instances of T type.
   */
  void setup(
      T && init_test_type,
      std::function <void (const T &, const T &)> & compare_func_handle)
  {
    rosidl_message_type_support_ = rosidl_typesupport_cpp::get_message_type_support_handle<T>();
    rosidl_message_type_support_ = get_message_typesupport_handle(
      rosidl_message_type_support_, ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP__IDENTIFIER_VALUE);

    message_type_support_callbacks_ = static_cast<const message_type_support_callbacks_t *>(
      rosidl_message_type_support_->data);

    tested_type_ = std::move(init_test_type);
    compare_func_handle_ = &compare_func_handle;
  }

  /*
   * @brief Checks for correctness of the retrieved typesupport's identifier.
   */
  void check_identifier()
  {
    ASSERT_EQ(strcmp(rosidl_message_type_support_->typesupport_identifier,
      ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP__IDENTIFIER_VALUE), 0);
  }

  /*
   * @brief Tests serialization and deserialization of a T type message,
   *        and compares the original instance and the deserialized one
   *        in terms of equalness.
   */
  void test_serialize_deserialize()
  {
    ucdrBuffer mb_writer;
    ucdrBuffer mb_reader;

    uint8_t mb_buffer[5000];
    ucdr_init_buffer(&mb_writer, mb_buffer, sizeof(mb_buffer));
    ucdr_init_buffer(&mb_reader, mb_buffer, sizeof(mb_buffer));

    ASSERT_TRUE(message_type_support_callbacks_->cdr_serialize(&tested_type_, &mb_writer));

    T test_out;
    ASSERT_TRUE(message_type_support_callbacks_->cdr_deserialize(&mb_reader, &test_out));

    compare(tested_type_, test_out);
  }

protected:
  /*
   * @brief Compare two given instances of the same type (T), in terms of equalness.
   * @param[in] A First instance to be compared.
   * @param[in] B Second instance to be compared.
   */
  void compare(
      const T & A,
      const T & B)
  {
    (*compare_func_handle_)(A, B);
  }

  T tested_type_;
  const rosidl_message_type_support_t * rosidl_message_type_support_;
  const message_type_support_callbacks_t * message_type_support_callbacks_;
  std::function<void (const T &, const T &)> * compare_func_handle_;
};

/******************************************************************************
                      Serialize/deserialize test suites.
 *****************************************************************************/

/*
 * @brief Primitive ROS 2 types serialization and deserialization tests.
 */
template <typename T>
class PrimitivesTestTypeSupport : public TestTypeSupport<T> {};

TYPED_TEST_CASE(PrimitivesTestTypeSupport,
  testing::Types<rosidl_typesupport_microxrcedds_test_msg::msg::Primitive>);
TYPED_TEST(PrimitivesTestTypeSupport, serialize_primitive_types)
{
  std::function<void (
      const rosidl_typesupport_microxrcedds_test_msg::msg::Primitive &,
      const rosidl_typesupport_microxrcedds_test_msg::msg::Primitive &)> compare_primitives ([](
        const rosidl_typesupport_microxrcedds_test_msg::msg::Primitive & A,
        const rosidl_typesupport_microxrcedds_test_msg::msg::Primitive & B) -> void
  {
    EXPECT_EQ(A.bool_test, B.bool_test);
    EXPECT_EQ(A.byte_test, B.byte_test);
    EXPECT_EQ(A.char_test, B.char_test);
    EXPECT_EQ(A.float32_test, B.float32_test);
    EXPECT_EQ(A.double_test, B.double_test);
    EXPECT_EQ(A.int8_test, B.int8_test);
    EXPECT_EQ(A.uint8_test, B.uint8_test);
    EXPECT_EQ(A.int16_test, B.int16_test);
    EXPECT_EQ(A.uint16_test, B.uint16_test);
    EXPECT_EQ(A.int32_test, B.int32_test);
    EXPECT_EQ(A.uint32_test, B.uint32_test);
    EXPECT_EQ(A.int64_test, B.int64_test);
    EXPECT_EQ(A.uint64_test, B.uint64_test);

    EXPECT_EQ(A.nested_test.unbounded_string1.compare(B.nested_test.unbounded_string1), 0);
    EXPECT_EQ(A.nested_test.unbounded_string2.compare(B.nested_test.unbounded_string2), 0);
    EXPECT_EQ(A.nested_test.unbounded_string3.compare(B.nested_test.unbounded_string3), 0);
    EXPECT_EQ(A.nested_test.unbounded_string4.compare(B.nested_test.unbounded_string4), 0);
  });

  // Initialize data to be serialized and deserialized
  rosidl_typesupport_microxrcedds_test_msg::msg::Primitive init_primitive;
  init_primitive.bool_test = 0x01;
  init_primitive.byte_test = 0x01;
  init_primitive.char_test = 0x01;
  init_primitive.float32_test = 100.001;
  init_primitive.double_test = 100.001;
  init_primitive.int8_test = 0x01;
  init_primitive.uint8_test = 0x01;
  init_primitive.int16_test = 0x0101;
  init_primitive.uint16_test = 0x0101;
  init_primitive.int32_test = 0x01010101;
  init_primitive.uint32_test = 0x01010101;
  init_primitive.int64_test = 0x0101010101010101;
  init_primitive.uint64_test = 0x0101010101010101;
  init_primitive.nested_test.unbounded_string1 = "ABCDEF";
  init_primitive.nested_test.unbounded_string2 = "TGHIJKLMNO";
  init_primitive.nested_test.unbounded_string3 = "PQRSTVWX";
  init_primitive.nested_test.unbounded_string4 = "TYZ0123456789";

  this->setup(std::move(init_primitive), compare_primitives);
  this->check_identifier();
  this->test_serialize_deserialize();
}
