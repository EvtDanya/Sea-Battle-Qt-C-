#include "statistics.h"

Statistics::Statistics(): unfinishedGames(0), winsEasyBot(0), winsMediumBot(0), winsMultiplayer(0), defeatsEasyBot(0), defeatsMediumBot(0), defeatsMultiplayer(0), rank(0)
{

}

void Statistics::fromJson(const json &j, Statistics &stat)
{
    stat.rank = j.at("rank").get<uint8_t>();
    stat.unfinishedGames = j.at("unfinished games").get<uint8_t>();
    stat.winsEasyBot = j.at("wins easy bot").get<uint16_t>();
    stat.winsMediumBot = j.at("wins medium bot").get<uint16_t>();
    stat.winsMultiplayer = j.at("wins multiplayer").get<uint16_t>();
    stat.defeatsEasyBot = j.at("defeats easy bot").get<uint16_t>();
    stat.defeatsMediumBot = j.at("defeats medium bot").get<uint16_t>();
    stat.defeatsMultiplayer = j.at("defeats multiplayer").get<uint16_t>();
}

void Statistics::toJson(json &j, const Statistics &stat)
{
    j = json
    {
        { "rank", stat.rank},
        { "unfinished games", stat.unfinishedGames},
        { "wins easy bot", stat.winsEasyBot},
        { "wins medium bot", stat.winsMediumBot},
        { "wins multiplayer", stat.winsMultiplayer},
        { "defeats easy bot", stat.defeatsEasyBot},
        { "defeats medium bot", stat.defeatsMediumBot},
        { "defeats multiplayer", stat.defeatsMultiplayer},
    };
}

void Statistics::saveToFile()
{
    json data;
    toJson(data, *this);

    std::ofstream file("statistics\\stat.json");

    if (file.is_open())
    {
        file << data << std::endl;
    }

    file.close();
}

void Statistics::loadFromFile()
{
    json data;
    std::ifstream file("statistics\\stat.json");

    if (file.is_open())
    {
        file >> data;
        fromJson(data, *this);
    }

    file.close();
}

void Statistics::reset()
{
    this->unfinishedGames = 0;
    this->winsEasyBot = 0;
    this->winsMediumBot = 0;
    this->winsMultiplayer = 0;
    this->defeatsEasyBot = 0;
    this->defeatsMediumBot = 0;
    this->defeatsMultiplayer = 0;
    this->rank = 0;
}

Config::Config(): volume(100)
{

}

void Config::fromJson(const json &j, Config &config)
{
    config.volume = j.at("volume").get<uint8_t>();
}

void Config::toJson(json &j, const Config &config)
{
    j = json
    {
        { "volume", config.volume}
    };
}

void Config::saveToFile()
{
    json data;
    toJson(data, *this);

    std::ofstream file("config\\config.json");

    if (file.is_open())
    {
        file << data << std::endl;
    }

    file.close();
}

void Config::loadFromFile()
{
    json data;
    std::ifstream file("config\\config.json");

    if (file.is_open())
    {
        file >> data;
        fromJson(data, *this);
    }

    file.close();
}


