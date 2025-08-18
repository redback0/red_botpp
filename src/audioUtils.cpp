
#include <filesystem>
#include <mpg123.h>
#include <out123.h>
#include <string>
#include <vector>
#include <iostream>

std::vector<uint8_t> getDecodedAudio(std::string file_name)
{
    // code sourced from dpp example -- needs to be checked/logging added
    std::vector<uint8_t> pcmdata;

    mpg123_init();

    int err = 0;
    unsigned char* buffer;
    size_t buffer_size, done;
    int channels, encoding;
    long rate;

    mpg123_handle *mh = mpg123_new(NULL, &err);
    mpg123_param(mh, MPG123_FORCE_RATE, 48000, 48000.0);

    buffer_size = mpg123_outblock(mh);
    buffer = new unsigned char[buffer_size];

    mpg123_open(mh, file_name.c_str());
    mpg123_getformat(mh, &rate, &channels, &encoding);

    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
    {
        for (size_t i = 0; i < buffer_size; i++)
        {
            pcmdata.push_back(buffer[i]);
        }
    }
    delete[] buffer;
    mpg123_close(mh);
    mpg123_delete(mh);

    return pcmdata;
}