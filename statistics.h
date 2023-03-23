#pragma once

#include <fstream>
#include <QDebug>

#include "libs/json.hpp"
using json = nlohmann::json;

enum class RankChecked
{
    NewRank, Nothing, Downgrading
};

class Statistics
{
public:
    Statistics();

    void fromJson(const json& j, Statistics& stat);
    void toJson(json& j, const Statistics& stat);

    void saveToFile();
    void loadFromFile();

    void reset();

    uint16_t unfinishedGames;
    uint16_t winsEasyBot;
    uint16_t winsMediumBot;
    uint16_t winsMultiplayer;
    uint16_t defeatsEasyBot;
    uint16_t defeatsMediumBot;
    uint16_t defeatsMultiplayer;
    uint8_t rank;
};

class Config
{
public:
    Config();

    void fromJson(const json& j, Config& config);
    void toJson(json& j, const Config& config);

    void loadFromFile();

    uint8_t volume;

public slots:
    void saveToFile();
};
