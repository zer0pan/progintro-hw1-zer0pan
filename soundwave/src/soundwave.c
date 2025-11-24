#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// read 1 byte or die
int read_byte()
{
    int c = getchar();
    if (c == EOF)
    {
        fprintf(stderr, "Error! unexpected EOF\n");
        exit(1);
    }
    return c;
}

// read 4-byte little-endian unsigned integer
unsigned int read_u32()
{
    int b0 = read_byte();
    int b1 = read_byte();
    int b2 = read_byte();
    int b3 = read_byte();
    return (unsigned int)(b0 | (b1 << 8) | (b2 << 16) | (b3 << 24));
}

// read 2-byte little-endian unsigned integer
unsigned int read_u16()
{
    int b0 = read_byte();
    int b1 = read_byte();
    return (unsigned int)(b0 | (b1 << 8));
}

void write_u16(unsigned int x)
{
    putchar(x & 0xFF);
    putchar((x >> 8) & 0xFF);
}

void write_u32(unsigned int x)
{
    putchar(x & 0xFF);
    putchar((x >> 8) & 0xFF);
    putchar((x >> 16) & 0xFF);
    putchar((x >> 24) & 0xFF);
}

// read 4-byte tag like "RIFF", "WAVE", "fmt ", "data"
void must_read_tag(const char *tag)
{
    for (int i = 0; i < 4; i++)
    {
        int c = read_byte();
        if (c != tag[i])
        {
            fprintf(stderr, "Error! \"%s\" not found\n", tag);
            exit(1);
        }
    }
}

void read_wav_header(
    unsigned int *SizeOfFile,
    unsigned int *FmtChunkSize,
    unsigned int *WaveType,
    unsigned int *MonoStereo,
    unsigned int *SampleRate,
    unsigned int *BytesPerSec,
    unsigned int *BlockAlign,
    unsigned int *BitsPerSample,
    unsigned int *SizeOfData)
{
    // "RIFF"
    must_read_tag("RIFF");

    *SizeOfFile = read_u32();

    // "WAVE"
    must_read_tag("WAVE");

    // "fmt "
    must_read_tag("fmt ");

    *FmtChunkSize = read_u32();
    if (*FmtChunkSize != 16)
    {
        fprintf(stderr, "Error! size of format chunk should be 16\n");
        exit(1);
    }

    *WaveType = read_u16();
    if (*WaveType != 1)
    {
        fprintf(stderr, "Error! WAVE type format should be 1\n");
        exit(1);
    }

    *MonoStereo = read_u16();
    if (!(*MonoStereo == 1 || *MonoStereo == 2))
    {
        fprintf(stderr, "Error! mono/stereo should be 1 or 2\n");
        exit(1);
    }

    *SampleRate = read_u32();

    *BytesPerSec = read_u32();

    *BlockAlign = read_u16();

    *BitsPerSample = read_u16();

    // validation
    if (*BytesPerSec != (*SampleRate) * (*BlockAlign))
    {
        fprintf(stderr, "Error! bytes/second should be sample rate x block alignment\n");
        exit(1);
    }

    if (!(*BitsPerSample == 8 || *BitsPerSample == 16))
    {
        fprintf(stderr, "Error! bits/sample should be 8 or 16\n");
        exit(1);
    }

    unsigned int expectedBA = (*BitsPerSample / 8) * (*MonoStereo);
    if (*BlockAlign != expectedBA)
    {
        fprintf(stderr, "Error! block alignment should be bits per sample / 8 x mono/stereo\n");
        exit(1);
    }

    // "data"
    must_read_tag("data");

    *SizeOfData = read_u32();
}

// ----------implementaton of info------------------
void do_info()
{
    unsigned int SizeOfFile, FmtChunkSize, WaveType;
    unsigned int MonoStereo, SampleRate, BytesPerSec;
    unsigned int BlockAlign, BitsPerSample, SizeOfData;

    // "RIFF"
    must_read_tag("RIFF");

    SizeOfFile = read_u32();
    printf("size of file: %u\n", SizeOfFile);

    // "WAVE"
    must_read_tag("WAVE");

    // "fmt "
    must_read_tag("fmt ");

    FmtChunkSize = read_u32();
    if (FmtChunkSize != 16)
    {
        fprintf(stderr, "Error! size of format chunk should be 16\n");
        exit(1);
    }
    printf("size of format chunk: %u\n", FmtChunkSize);

    WaveType = read_u16();
    if (WaveType != 1)
    {
        fprintf(stderr, "Error! WAVE type format should be 1\n");
        exit(1);
    }
    printf("WAVE type format: %u\n", WaveType);

    MonoStereo = read_u16();
    if (!(MonoStereo == 1 || MonoStereo == 2))
    {
        fprintf(stderr, "Error! mono/stereo should be 1 or 2\n");
        exit(1);
    }
    printf("mono/stereo: %u\n", MonoStereo);

    SampleRate = read_u32();
    printf("sample rate: %u\n", SampleRate);

    BytesPerSec = read_u32();
    printf("bytes/sec: %u\n", BytesPerSec);

    BlockAlign = read_u16();
    printf("block alignment: %u\n", BlockAlign);

    BitsPerSample = read_u16();
    printf("bits/sample: %u\n", BitsPerSample);

    // validation
    if (BytesPerSec != (SampleRate) * (BlockAlign))
    {
        fprintf(stderr, "Error! bytes/second should be sample rate x block alignment\n");
        exit(1);
    }

    if (!(BitsPerSample == 8 || BitsPerSample == 16))
    {
        fprintf(stderr, "Error! bits/sample should be 8 or 16\n");
        exit(1);
    }

    unsigned int expectedBA = (BitsPerSample / 8) * (MonoStereo);
    if (BlockAlign != expectedBA)
    {
        fprintf(stderr, "Error! block alignment should be bits per sample / 8 x mono/stereo\n");
        exit(1);
    }

    // "data"
    must_read_tag("data");

    SizeOfData = read_u32();
    printf("size of data chunk: %u\n", SizeOfData);

    // We have already read 44 bytes of header up to SizeOfData.
    // Total expected size according to RIFF:
    unsigned int expected_total = SizeOfFile + 8; // with RIFF and SizeOfFile bytes

    // We have currently read: 44 bytes (fixed WAV header size)
    unsigned int bytes_read = 44;

    // Now read *exactly* SizeOfData bytes (the data chunk)
    for (unsigned int i = 0; i < SizeOfData; i++)
    {
        int c = getchar();
        if (c == EOF)
        {
            fprintf(stderr, "Error! insufficient data\n");
            exit(1);
        }
        bytes_read++;
    }

    // Now read ANY extra chunks until EOF
    int c;
    while ((c = getchar()) != EOF)
    {
        bytes_read++;
    }

    // Final validation: does actual file size match RIFF size?
    if (bytes_read < expected_total)
    {
        fprintf(stderr, "Error! insufficient data\n");
        exit(1);
    }

    if (bytes_read > expected_total)
    {
        fprintf(stderr, "Error! bad file size (found data past the expected end of file)\n");
        exit(1);
    }

    exit(0);
}

void do_channel(const char *chan)
{
    unsigned int SizeOfFile, FmtChunkSize, WaveType;
    unsigned int MonoStereo, SampleRate, BytesPerSec;
    unsigned int BlockAlign, BitsPerSample, SizeOfData;

    read_wav_header(
        &SizeOfFile, &FmtChunkSize, &WaveType,
        &MonoStereo, &SampleRate, &BytesPerSec,
        &BlockAlign, &BitsPerSample, &SizeOfData);

    /* We assume stereo because mono handled separately */
    unsigned int bytesPerChannel = BitsPerSample / 8;

    unsigned int newMonoStereo = 1;
    unsigned int newBlockAlign = bytesPerChannel;
    unsigned int newBytesPerSec = SampleRate * newBlockAlign;
    unsigned int newSizeOfData = SizeOfData / 2;

    unsigned int newSizeOfFile =
        SizeOfFile - SizeOfData + newSizeOfData;

    /* -------- Write updated header -------- */

    putchar('R');
    putchar('I');
    putchar('F');
    putchar('F');
    write_u32(newSizeOfFile);

    putchar('W');
    putchar('A');
    putchar('V');
    putchar('E');

    putchar('f');
    putchar('m');
    putchar('t');
    putchar(' ');

    write_u32(FmtChunkSize);  // 16
    write_u16(WaveType);      // 1
    write_u16(newMonoStereo); // NOW 1
    write_u32(SampleRate);
    write_u32(newBytesPerSec);
    write_u16(newBlockAlign);
    write_u16(BitsPerSample);

    putchar('d');
    putchar('a');
    putchar('t');
    putchar('a');
    write_u32(newSizeOfData);

    /* -------- Process samples -------- */

    if (strcmp(chan, "right") == 0)
    {
        for (unsigned int i = 0; i < SizeOfData; i += 2 * bytesPerChannel)
        {
            unsigned char L[4], R[4];

            for (int b = 0; b < bytesPerChannel; b++)
                L[b] = read_byte();

            for (int b = 0; b < bytesPerChannel; b++)
                R[b] = read_byte();

            // keep RIGHT channel
            for (int b = 0; b < bytesPerChannel; b++)
                putchar(R[b]);
        }
    }
    else if (strcmp(chan, "left") == 0)
    {
        for (unsigned int i = 0; i < SizeOfData; i += 2 * bytesPerChannel)
        {
            unsigned char L[4], R[4];

            for (int b = 0; b < bytesPerChannel; b++)
                L[b] = read_byte();

            for (int b = 0; b < bytesPerChannel; b++)
                R[b] = read_byte();

            // keep LEFT channel
            for (int b = 0; b < bytesPerChannel; b++)
                putchar(L[b]);
        }
    }
    else
    {
        fprintf(stderr, "left or right only\n");
        exit(1);
    }

    /* copy any trailing chunks */
    int d;
    while ((d = getchar()) != EOF)
        putchar(d);

    exit(0);
}

void do_rate(double factor)
{
    unsigned int SizeOfFile, FmtChunkSize, WaveType;
    unsigned int MonoStereo, SampleRate, BytesPerSec;
    unsigned int BlockAlign, BitsPerSample, SizeOfData;

    read_wav_header(
        &SizeOfFile, &FmtChunkSize, &WaveType,
        &MonoStereo, &SampleRate, &BytesPerSec,
        &BlockAlign, &BitsPerSample, &SizeOfData);

    unsigned int newSR = (unsigned int)(SampleRate * factor);
    unsigned int newBPS = newSR * BlockAlign;

    putchar('R');
    putchar('I');
    putchar('F');
    putchar('F');

    // file size (4 bytes little endian)
    write_u32(SizeOfFile);

    putchar('W');
    putchar('A');
    putchar('V');
    putchar('E');

    // "fmt "
    putchar('f');
    putchar('m');
    putchar('t');
    putchar(' ');

    write_u32(FmtChunkSize);

    write_u16(WaveType);
    write_u16(MonoStereo);
    write_u32(newSR);  // changed
    write_u32(newBPS); // changed
    write_u16(BlockAlign);
    write_u16(BitsPerSample);

    // "data"
    putchar('d');
    putchar('a');
    putchar('t');
    putchar('a');
    write_u32(SizeOfData);

    for (int i = 0; i < SizeOfData; i++)
    {
        int c = read_byte();
        putchar(c);
    }

    int d;
    while ((d = getchar()) != EOF)
        putchar(d);

    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage : $ ./soundwave [option] < <file>.wav \n");
        printf("type ./soundwave --help to see options\n");
        return 1;
    }

    if (argc == 2)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            printf("options are : info , rate , channel , volume \n");
            printf("$ ./soundwave info < myfile.wav \n");
            printf("$ ./soundwave rate 1.10 < parios.wav > mimerotas.wav \n");
            printf("$ ./soundwave channel left < CaliforniaDreamin.wav > CaliforniaDreamin4.wav \n");
            return 0;
        }
        else if (strcmp(argv[1], "info") == 0)
        {
            do_info();
        }
        else
        {
            printf("type ./soundwave --help to see options\n");
            return 1;
        }
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1], "rate") == 0)
        {
            double factor = (double)atof(argv[2]);
            do_rate(factor);
        }
        else if (strcmp(argv[1], "channel") == 0)
        {
            do_channel(argv[2]);
        }
        else
        {
            printf("type ./soundwave --help to see options\n");
            return 1;
        }
    }
    else
    {
        printf("options are : info , rate , channel , volume \n");
        printf("$ ./soundwave info < myfile.wav \n");
        printf("$ ./soundwave rate 1.10 < parios.wav > mimerotas.wav \n");
        printf("$ ./soundwave channel left < CaliforniaDreamin.wav > CaliforniaDreamin4.wav \n");
        return 1;
    }

    return 0;
}