#ifndef LIBTORRENT_CHUNK_MANAGER_H
#define LIBTORRENT_CHUNK_MANAGER_H

#include <vector>
#include <torrent/common.h>

namespace torrent {

// TODO: Currently all chunk lists are inserted, despite the download
// not being open/active.

class LIBTORRENT_EXPORT ChunkManager : private std::vector<ChunkList*> {
public:
  using base_type = std::vector<ChunkList*>;
  using size_type = uint32_t;

  using base_type::iterator;
  using base_type::reverse_iterator;
  using base_type::const_iterator;

  using base_type::begin;
  using base_type::end;
  using base_type::size;
  using base_type::empty;

  ChunkManager();
  ~ChunkManager();

  uint64_t            memory_usage() const                      { return m_memoryUsage; }
  uint64_t            sync_queue_memory_usage() const;

  uint32_t            memory_block_count() const                { return m_memoryBlockCount; }
  uint32_t            sync_queue_size() const;

  // Should we allow the client to reserve some memory?

  // The client should set this automatically if ulimit is set.
  uint64_t            max_memory_usage() const                  { return m_maxMemoryUsage; }
  void                set_max_memory_usage(uint64_t bytes)      { m_maxMemoryUsage = bytes; }

  // Estimate the max memory usage possible, capped at 1GB.
  static uint64_t     estimate_max_memory_usage();

  uint64_t            safe_free_diskspace() const;

  bool                safe_sync() const                         { return m_safeSync; }
  void                set_safe_sync(uint32_t state)             { m_safeSync = state; }

  // Set the interval to wait after the last write to a chunk before
  // trying to sync it. By not forcing a sync too early it should give
  // the kernel an oppertunity to sync at its convenience.
  uint32_t            timeout_sync() const                      { return m_timeoutSync; }
  void                set_timeout_sync(uint32_t seconds)        { m_timeoutSync = seconds; }

  uint32_t            timeout_safe_sync() const                 { return m_timeoutSafeSync; }
  void                set_timeout_safe_sync(uint32_t seconds)   { m_timeoutSafeSync = seconds; }

  // Set to 0 to disable preloading.
  //
  // How the value is used is yet to be determined, but it won't be
  // able to use actual requests in the request queue as we can easily
  // stay ahead of it causing preloading to fail.
  uint32_t            preload_type() const                      { return m_preloadType; }
  void                set_preload_type(uint32_t t)              { m_preloadType = t; }

  uint32_t            preload_min_size() const                  { return m_preloadMinSize; }
  void                set_preload_min_size(uint32_t bytes)      { m_preloadMinSize = bytes; }

  // Required rate before attempting to preload chunk, per whole
  // megabyte of chunk size.
  uint32_t            preload_required_rate() const             { return m_preloadRequiredRate; }
  void                set_preload_required_rate(uint32_t bytes) { m_preloadRequiredRate = bytes; }


  void                insert(ChunkList* chunkList);
  void                erase(ChunkList* chunkList);

  // The client may use these functions to affect the library's memory
  // usage by indicating how much it uses. This shouldn't really be
  // nessesary unless the client maps large amounts of memory.
  //
  // If the caller finds out the allocated memory quota isn't needed
  // due to e.g. other errors then 'deallocate_unused' must be called
  // within the context of the original 'allocate' caller in order to
  // properly be reflected when logging.
  //
  // The primary user of these functions is ChunkList.

  static constexpr int allocate_revert_log = (1 << 0);
  static constexpr int allocate_dont_log   = (1 << 1);

  bool                allocate(uint32_t size, int flags = 0);
  void                deallocate(uint32_t size, int flags = 0);

  void                try_free_memory(uint64_t size);

  void                periodic_sync();

  // Not sure if I wnt these here. Consider implementing a generic
  // statistics API.
  uint32_t            stats_preloaded() const                   { return m_statsPreloaded; }
  void                inc_stats_preloaded()                     { m_statsPreloaded++; }

  uint32_t            stats_not_preloaded() const               { return m_statsNotPreloaded; }
  void                inc_stats_not_preloaded()                 { m_statsNotPreloaded++; }

private:
  ChunkManager(const ChunkManager&) = delete;
  ChunkManager& operator=(const ChunkManager&) = delete;

  void                sync_all(int flags, uint64_t target) LIBTORRENT_NO_EXPORT;

  uint64_t            m_memoryUsage{0};
  // 1/5 of the available memory should be enough for the client. If
  // the client really requires alot more memory it should call this
  // itself.
  uint64_t            m_maxMemoryUsage{(estimate_max_memory_usage() * 4) / 5};

  uint32_t            m_memoryBlockCount{0};

  bool                m_safeSync{false};
  uint32_t            m_timeoutSync{600};
  uint32_t            m_timeoutSafeSync{900};

  uint32_t            m_preloadType{0};
  uint32_t            m_preloadMinSize{256 << 10};
  uint32_t            m_preloadRequiredRate{5 << 10};

  uint32_t            m_statsPreloaded{0};
  uint32_t            m_statsNotPreloaded{0};

  int32_t             m_timerStarved{0};
  size_type           m_lastFreed{0};
};

} // namespace torrent

#endif
