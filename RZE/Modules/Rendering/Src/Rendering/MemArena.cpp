#include <Rendering/MemArena.h>

#include <Utils/DebugUtils/Debug.h>
#include <Utils/PrimitiveDefs.h>

namespace Rendering
{
	namespace MemArena
	{
		Byte* s_memory = nullptr;

		struct ArenaState
		{
			size_t curPos = 0;
			size_t size = 0;
		} s_arenaState;

		void ValidateArenaState(size_t sizeRequested)
		{
			// validate state. can't alloc if these rules arent met.
			if (s_arenaState.curPos + sizeRequested > s_arenaState.size)
			{
				size_t sizeAvailable = s_arenaState.size - s_arenaState.curPos;
				RZE_LOG_ARGS("Rendering memory arena has run out of space. Size requested: %zu available: %zu ", sizeRequested, sizeAvailable);
				AssertExpr(s_arenaState.curPos + sizeRequested < s_arenaState.size);
			}
		}

		void InitializeArena(size_t size)
		{
			if (s_memory != nullptr)
			{
				RZE_LOG("Rendering memory arena being re-initialized. \
					This is incorrect as the memory should be valid once for the length of a Rendering::Renderer lifetime.");
				AssertIsNull(s_memory);
			}
			
			s_memory = new Byte[size];

			s_arenaState.size = size;
		}

		void* Alloc(size_t size)
		{
			ValidateArenaState(size);

			Byte* allocMem = &s_memory[s_arenaState.curPos];
			s_arenaState.curPos += size;

			return allocMem;
		}

		void* Get()
		{
			return s_memory;
		}

		void Cycle()
		{
			s_arenaState.curPos = 0;
		}
	}
}