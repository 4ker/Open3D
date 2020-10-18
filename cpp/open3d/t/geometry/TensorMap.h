// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <unordered_map>

#include "open3d/core/Tensor.h"

namespace open3d {
namespace t {
namespace geometry {

/// Map of string to Tensor. Used to store attributes geometries.
///
/// The primary key's tensor is the primary tensor.
/// e.g.
/// tensor_map.primary_key: "points"
/// tensor_map["points"]  : Tensor of shape {100, 3}.
/// tensor_map["colors"]  : Tensor of shape {100, 3}.
/// tensor_map["normals"] : Tensor of shape {100, 3}.
///
/// Typically, tensors in the TensorMap should have the same length (the first
/// dimension of shape) and device as the primary tensor.
class TensorMap : public std::unordered_map<std::string, core::Tensor> {
public:
    /// Create empty TensorMap and set primary key.
    TensorMap(const std::string& primary_key)
        : std::unordered_map<std::string, core::Tensor>(),
          primary_key_(primary_key) {}

    /// Create TensorMap with pre-populated values.
    TensorMap(const std::string& primary_key,
              const std::unordered_map<std::string, core::Tensor>&
                      map_keys_to_tensor)
        : std::unordered_map<std::string, core::Tensor>(),
          primary_key_(primary_key) {
        Assign(map_keys_to_tensor);
    }

    /// A primary key is always required. This constructor can be marked as
    /// delete in C++, but it is needed for pybind to bind as a generic python
    /// map interface.
    TensorMap() : TensorMap("Undefined") {
        utility::LogError("Please construct TensorMap with a primary key.");
    }

    /// Clear the current map and assign new keys and values. The primary key
    /// remains unchanged. The input \p map_keys_to_tensor must at least
    /// contain the primary key. Data won't be copied, tensors stored in the map
    /// share the same memory as the input map.
    ///
    /// \param map_keys_to_tensor. The keys and values to be assigned.
    void Assign(const std::unordered_map<std::string, core::Tensor>&
                        map_keys_to_tensor);

    /// Returns the primary key of the TensorMap.
    std::string GetPrimaryKey() const { return primary_key_; }

    /// Returns true if all tensors in the map have the same size.
    bool IsSizeSynchronized() const;

    /// Assert IsSizeSynchronized().
    void AssertSizeSynchronized() const;

    /// Returns true if the key exists in the map.
    /// Same as C++20's std::unordered_map::contains().
    bool Contains(const std::string& key) const { return count(key) != 0; }

private:
    /// Asserts that \p map_keys_to_tensors has the same keys as the
    /// TensorMap.
    ///
    /// \param map_keys_to_tensors A map of string to Tensor. Typically the map
    /// is used for SynchronizedPushBack.
    void AssertTensorMapSameKeys(
            const std::unordered_map<std::string, core::Tensor>&
                    map_keys_to_tensors) const;

    /// Asserts that all of the tensors in \p map_keys_to_tensors have the same
    /// device as the primary tensor.
    ///
    /// \param map_keys_to_tensors A map of string to Tensor. Typically the map
    /// is used for SynchronizedPushBack.
    void AssertTensorMapSameDevice(
            const std::unordered_map<std::string, core::Tensor>&
                    map_keys_to_tensors) const;

    /// Returns the size (length) of the primary key's tensor.
    int64_t GetPrimarySize() const { return at(primary_key_).GetShape()[0]; }

    /// Returns the device of the primary key's tensor.
    core::Device GetPrimaryDevice() const {
        return at(primary_key_).GetDevice();
    }

    /// Primary key of the TensorMap.
    std::string primary_key_;
};

}  // namespace geometry
}  // namespace t
}  // namespace open3d
