// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

use crate::pm::ProcessManager;
use ::alloc::collections::LinkedList;
use ::core::cell::RefCell;
use ::sys::{
    error::Error,
    pm::ThreadIdentifier,
};

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// A type that represents a condition variable.
///
pub struct Condvar {
    /// Threads that are sleeping on the condition variable.
    sleeping: RefCell<LinkedList<ThreadIdentifier>>,
}

//==================================================================================================
// Implementations
//==================================================================================================

impl Condvar {
    ///
    /// # Description
    ///
    /// Initializes a new condition variable.
    ///
    /// # Returns
    ///
    /// A new condition variable.
    ///
    pub fn new() -> Self {
        Self {
            sleeping: RefCell::new(LinkedList::new()),
        }
    }

    ///
    /// # Description
    ///
    /// Wakes a single thread that is waiting on the target condition variable.
    ///
    /// # Returns
    ///
    /// Upon successful completion, empty is returned. Otherwise, an error is returned instead.
    ///
    pub fn notify_first(&self) -> Result<(), Error> {
        if let Some(tid) = self.sleeping.borrow_mut().pop_front() {
            ProcessManager::wakeup(tid)?;
        }

        Ok(())
    }

    ///
    /// # Description
    ///
    /// Wakes up a specific thread that is waiting on the target condition variable.
    ///
    /// # Parameters
    ///
    /// - `tid`: Identifier of the target thread.
    ///
    /// # Returns
    ///
    /// Upon successful completion, empty is returned. Otherwise, an error is returned instead.
    ///
    pub fn notify_thread(&self, tid: ThreadIdentifier) -> Result<(), Error> {
        if self.sleeping.borrow_mut().iter().any(|&t| t == tid) {
            ProcessManager::wakeup(tid)?;
        }

        Ok(())
    }

    ///
    /// # Description
    ///
    /// Waits on the condition variable.
    ///
    pub fn wait(&self) -> Result<(), Error> {
        self.sleeping
            .borrow_mut()
            .push_back(ProcessManager::get_tid()?);

        ProcessManager::sleep()?;

        Ok(())
    }
}

unsafe impl Send for Condvar {}

unsafe impl Sync for Condvar {}
