// Unity text extractor for LangData assets (Yu-Gi-Oh! Cross Duel)
// by Xan / Tenjoin

#include <iostream>

bool ExtractLangdata(const char* inFilename, const char* outFilename)
{
    FILE* fin = fopen(inFilename, "rb");
    if (!fin)
    {
        std::cerr << "ERROR: Can't open file " << inFilename << " for reading!\n";
        perror("ERROR");
        return false;
    }

    // validate the header magic
    uint32_t magic = 0;
    fread(&magic, sizeof(uint32_t), 1, fin);
    if (magic != 0x474E414C)
    {
        fclose(fin);
        std::cerr << "ERROR: Invalid header magic on file: " << inFilename << "\n";
        return false;
    }

    FILE* fout = fopen(outFilename, "wb");
    if (!fout)
    {
        fclose(fin);
        std::cerr << "ERROR: Can't open file " << outFilename << " for writing!\n";
        perror("ERROR");
        return false;
    }

    uint32_t strcount = 0;
    fread(&strcount, sizeof(uint32_t), 1, fin);
    std::cout << "String count: " << strcount << "\n";

    for (uint32_t i = 0; i < strcount; i++)
    {
        uint32_t len = 0;
        uint8_t tst = 0;
        uint32_t unkval = 0;
        bool bContRead = 0;

        fread(&unkval, sizeof(uint32_t), 1, fin);
        fread(&len, sizeof(uint8_t), 1, fin);

        fprintf(fout, "[%X]\n", unkval);

        if (len)
        {
            // skip unwanted bytes
            while (!bContRead)
            {
                fread(&tst, sizeof(char), 1, fin);
                if ((tst > 0x1F) || (tst == 0xA))
                {
                    fseek(fin, -1, SEEK_CUR);
                    bContRead = true;
                }
                else
                {
                    len += (tst - 1) << 7;
                }
            }

            char* buf = (char*)malloc(len);
            memset(buf, 0, len);
            fread(buf, sizeof(char), len, fin);
            fwrite(buf, sizeof(char), len, fout);
            fputc('\n', fout);
            free(buf);
        }
    }
    fclose(fout);
    fclose(fin);

    return true;
}

int main(int argc, char* argv[])
{
    std::cout << "Unity LangData extractor\n";
    if (argc < 2)
    {
        std::cout << "USAGE: " << argv[0] << " InFilename [OutFilename]\n";
        return -1;
    }
    
    std::string outfname;

    if (argc < 3)
    {
        outfname = argv[1];
        outfname += "_ex.txt";
    }
    else
    {
        outfname = argv[argc - 1];
    }

    std::cout << "Writing to: " << outfname << "\n";


    if (!ExtractLangdata(argv[1], outfname.c_str()))
        return -1;
    
    return 0;
}

