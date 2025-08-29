#pragma once

#include "../Encoder.hpp"
#include <x86/Instructions.hpp>

namespace Encoder
{
    namespace x86
    {
        class Encoder : public ::Encoder::Encoder
        {
        public:
            Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser);
            ~Encoder() = default;

        protected:
            bool OptimizeOffsets(std::vector<Parser::Section>& parsedSections) override;

            std::vector<uint8_t> EncodeInstruction(const Parser::Instruction::Instruction& instruction, bool ignoreUnresolved = false) override;
            uint64_t GetSize(const Parser::Instruction::Instruction& instruction) override;
            std::vector<uint8_t> EncodePadding(size_t length) override;

        private:
            inline uint8_t getRex(bool W, bool R, bool X, bool B)
            {
                uint8_t rex = 0b01000000;
                if (W) rex |= 0b00001000;
                if (R) rex |= 0b00000100;
                if (X) rex |= 0b00000010;
                if (B) rex |= 0b00000001;
                return rex;
            }

            std::vector<uint8_t> EncodeControlInstruction(const Parser::Instruction::Instruction& instruction, bool ignoreUnresolved);
            std::vector<uint8_t> EncodeInterruptInstruction(const Parser::Instruction::Instruction& instruction, bool ignoreUnresolved);
            std::vector<uint8_t> EncodeFlagInstruction(const Parser::Instruction::Instruction& instruction, bool ignoreUnresolved);
            std::vector<uint8_t> EncodeStackInstruction(const Parser::Instruction::Instruction& instruction, bool ignoreUnresolved);
            std::vector<uint8_t> EncodeDataInstruction(const Parser::Instruction::Instruction& instruction, bool ignoreUnresolved);
        };
    }
}
