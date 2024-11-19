#include "bot.hpp"
#include "server.hpp"
#include "channel.hpp"

bool Channel::startHangman(const std::string& word) {
    if (hangmanActive) {
        std::cerr << "A Hangman game is already active in this channel.\n";
        return false;
    }

    hangmanWord = word;
    hangmanProgress = std::string(word.size(), '_');
    hangmanGuesses.clear();
    hangmanLives = 5;
    hangmanActive = true;

    notifyAllInChannel(this, ":localhost Hangman game started! Guess the word: " + hangmanProgress + "\r\n");
    return true;
}

bool Channel::guessLetter(char letter, std::string& resultMsg) {
    if (!hangmanActive) {
        resultMsg = "No Hangman game is active in this channel.";
        return false;
    }

    letter = std::tolower(letter);

    if (std::find(hangmanGuesses.begin(), hangmanGuesses.end(), letter) != hangmanGuesses.end()) {
        resultMsg = "Letter '" + std::string(1, letter) + "' has already been guessed.";
        return false;
    }

    hangmanGuesses.push_back(letter);
    bool correctGuess = false;

    for (size_t i = 0; i < hangmanWord.size(); ++i) {
        if (std::tolower(hangmanWord[i]) == letter) {
            hangmanProgress[i] = hangmanWord[i];
            correctGuess = true;
        }
    }

    if (correctGuess) {
        resultMsg = "Correct! Current word: " + hangmanProgress;
        notifyAllInChannel(this, ":localhost " + resultMsg + "\r\n");
    } else {
        --hangmanLives;

        std::stringstream ss;
        ss << hangmanLives;

        resultMsg = "Incorrect! Letter '" + std::string(1, letter) + "' is not in the word. Remaining lives: " + ss.str();
        notifyAllInChannel(this, ":localhost " + resultMsg + "\r\n");
    }

    if (hangmanProgress == hangmanWord) {
        notifyAllInChannel(this, ":localhost Congratulations! You've guessed the word: " + hangmanWord + "\r\n");
        resetHangman();
        return true;
    } else if (hangmanLives <= 0) {
        notifyAllInChannel(this, ":localhost Game over! The word was: " + hangmanWord + "\r\n");
        resetHangman();
        return true;
    }

    return true;
}

bool Channel::solveWord(const std::string& guess, std::string& resultMsg) {
    if (!hangmanActive) {
        resultMsg = "No Hangman game is active in this channel.";
        return false;
    }

    if (guess == hangmanWord) {
        notifyAllInChannel(this, ":localhost Correct! The word was: " + hangmanWord + "\r\n");
        resetHangman();
        return true;
    } else {
        --hangmanLives;

        std::stringstream ss;
        ss << hangmanLives;

        if (hangmanLives <= 0) {
            notifyAllInChannel(this, ":localhost Game over! The word was: " + hangmanWord + "\r\n");
            resetHangman();
            return false;
        } else {
            resultMsg = "Incorrect guess. Remaining lives: " + ss.str();
            notifyAllInChannel(this, ":localhost " + resultMsg + "\r\n");
            return false;
        }
    }
}

void Channel::resetHangman() {
    hangmanActive = false;
    hangmanWord.clear();
    hangmanProgress.clear();
    hangmanGuesses.clear();
    hangmanLives = 0;
}

void Server::aBotHangman(std::map<int, Client*>::iterator it, const std::string &channelName, std::stringstream &iss) {
    Channel* ch = findChannel(channelName);
    if (!ch) {
        sendMsgServ(ERR_NOSUCHCHANNEL(it->second->getNick(), channelName), it->first);
        return;
    }

    std::string action, wordOrLetter;
    iss >> action >> wordOrLetter;

    if (action == "start") {
        if (wordOrLetter.empty()) {
            sendMsgServ(RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, "Usage: /BOT hangman #chanel start <word>"), it->first);
            return;
        }

        if (ch->startHangman(wordOrLetter)) {
            sendMsgServ(RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, "Hangman game started! Guess the word!"), it->first);
        } else {
            sendMsgServ(RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, "A game is already active in this channel."), it->first);
        }
    } else if (action == "guess") {
        if (wordOrLetter.size() != 1 || !std::isalpha(wordOrLetter[0])) {
            sendMsgServ(RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, "Invalid guess. Please guess a single letter."), it->first);
            return;
        }

        std::string resultMsg;
        if (ch->guessLetter(wordOrLetter[0], resultMsg)) {
            sendMsgServ(RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, resultMsg), it->first);
        } else {
            sendMsgServ(RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, resultMsg), it->first);
        }
    } else if (action == "solve") {
        if (wordOrLetter.empty()) {
            sendMsgServ(RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, "Usage: /BOT hangman #chanel solve <word>"), it->first);
            return;
        }

        std::string resultMsg;
        if (ch->solveWord(wordOrLetter, resultMsg)) {
            sendMsgServ(RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, "Game result: " + resultMsg), it->first);
        } else {
            sendMsgServ(RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, "Game result: " + resultMsg), it->first);
        }
    } else {
        sendMsgServ(RPL_PRIVMSG(user_info("BoTony", "BotUser"), channelName, "Unknown Hangman command. Use: start, guess, or solve"), it->first);
    }
}
