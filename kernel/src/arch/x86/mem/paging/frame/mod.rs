// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

mod number;

//==================================================================================================
// Exports
//==================================================================================================

pub use number::FrameNumber;

//==================================================================================================
// Unit Tests
//==================================================================================================

/// Run tests for this module.
pub fn test() -> bool {
    let mut passed: bool = true;

    passed &= number::test();

    passed
}
