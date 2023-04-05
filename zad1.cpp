#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <cmath>
#include <algorithm>
#include <sstream>

//Select mode
//Default mix
// "mix" "lowest"  "highest"  "mix-saturate
std::string mode = "";

class Color {
public:
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    unsigned int alpha;

    Color() : red(0), green(0), blue(0), alpha(0) {}
    Color(unsigned int r, unsigned int g, unsigned int b, unsigned int a)
        : red(r), green(g), blue(b), alpha(a) {}
};

bool isValidHexColor(const std::string& color) {
    std::regex hex_color_pattern("^([0-9a-fA-F]{3}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})$");
    return std::regex_match(color, hex_color_pattern);
}

bool isValidDecimalColor(const std::string& color) {
    std::regex decimal_color_pattern("^([0-9]{1,3},){3}[0-9]{1,3}$");
    return std::regex_match(color, decimal_color_pattern);
}

Color parseHexColor(const std::string& color) {
    unsigned int r, g, b, a = 255;

    if (color.length() == 3) {
        r = std::stoi(color.substr(0, 1), nullptr, 16) * 17;
        g = std::stoi(color.substr(1, 1), nullptr, 16) * 17;
        b = std::stoi(color.substr(2, 1), nullptr, 16) * 17;
    }
    else if (color.length() == 6) {
        r = std::stoi(color.substr(0, 2), nullptr, 16);
        g = std::stoi(color.substr(2, 2), nullptr, 16);
        b = std::stoi(color.substr(4, 2), nullptr, 16);
    }
    else if (color.length() == 8) {
        r = std::stoi(color.substr(0, 2), nullptr, 16);
        g = std::stoi(color.substr(2, 2), nullptr, 16);
        b = std::stoi(color.substr(4, 2), nullptr, 16);
        a = std::stoi(color.substr(6, 2), nullptr, 16);
    }

    return Color(r, g, b, a);
}

Color parseDecimalColor(const std::string& color) {
    std::stringstream ss(color);
    std::string token;
    std::vector<int> rgba_values;

    while (std::getline(ss, token, ',')) {
        rgba_values.push_back(std::stoi(token));
    }

    return Color(rgba_values[0], rgba_values[1], rgba_values[2], rgba_values[3]);
}
/**/
std::vector<Color> readColorsFromFile(const std::string& file_name) {
    std::ifstream file(file_name);
    std::vector<Color> colors;
    std::string line;



    if (file.is_open()) {
        while (std::getline(file, line)) {
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            if (isValidHexColor(line)) {
                colors.push_back(parseHexColor(line));
            }
            else if (isValidDecimalColor(line)) {
                colors.push_back(parseDecimalColor(line));
            }
            
        }
        
        file.close();
    }
    else {
        std::cerr << "Unable to open file '" << file_name << "'." << std::endl;
    }

    return colors;
}



std::vector<Color> cli_colors;

void parseArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--mode" || arg == "-m") && i + 1 < argc) {
            mode = argv[++i];
        }
        else if (isValidHexColor(arg)) {
            cli_colors.push_back(parseHexColor(arg));
        }
        else if (isValidDecimalColor(arg)) {
            cli_colors.push_back(parseDecimalColor(arg));
        }
    }
}

Color mixColors(const std::vector<Color>& colors) {
    unsigned int r = 0, g = 0, b = 0, a = 0;

    for (const Color& color : colors) {
        r += color.red;
        g += color.green;
        b += color.blue;
        a += color.alpha;
       
    }

    r /= colors.size();
    g /= colors.size();
    b /= colors.size();
    a /= colors.size();

    return Color(r, g, b, a);
}

Color lowestColors(const std::vector<Color>& colors) {
    unsigned int r = 255, g = 255, b = 255, a = 255;

    for (const Color& color : colors) {
        r = std::min(r, color.red);
        g = std::min(g, color.green);
        b = std::min(b, color.blue);
        a = std::min(a, color.alpha);
    }

    return Color(r, g, b, a);
}

Color highestColors(const std::vector<Color>& colors) {
    unsigned int r = 0, g = 0, b = 0, a = 0;

    for (const Color& color : colors) {
        r = std::max(r, color.red);
        g = std::max(g, color.green);
        b = std::max(b, color.blue);
        a = std::max(a, color.alpha);
    }

    return Color(r, g, b, a);
}

float hueToRgb(float p, float q, float t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1 / 6.0) return p + (q - p) * 6 * t;
    if (t < 1 / 2.0) return q;
    if (t < 2 / 3.0) return p + (q - p) * (2 / 3.0 - t) * 6;
    return p;
}

void rgbToHsl(const Color& color, float& h, float& s, float& l) {
    float r = color.red / 255.0;
    float g = color.green / 255.0;
    float b = color.blue / 255.0;

    float max = std::max(std::max(r, g), b);
    float min = std::min(std::min(r, g), b);
    l = (max + min) / 2;

    if (max == min) {
        h = s = 0;
    }
    else {
        float d = max - min;
        s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
        if (max == r) {
            h = (g - b) / d + (g < b ? 6 : 0);
          
        }
        else if (max == g) {
            h = (b - r) / d + 2;
        }
        else {
            h = (r - g) / d + 4;
        }
        h /= 6;
    }
}

Color mixSaturateColors(const std::vector<Color>& colors) {
    Color mixed = mixColors(colors);
    float h = 0, s = 0, l = 0;
    rgbToHsl(mixed, h, s, l);

    float average_saturation = 0;
    for (const Color& color : colors) {
        float temp_h, temp_s, temp_l;
        rgbToHsl(color, temp_h, temp_s, temp_l);
        average_saturation += temp_s;
    }
    average_saturation /= colors.size();

    return Color(mixed.red, mixed.green, mixed.blue, static_cast<unsigned int>(average_saturation * 255));
}

int main(int argc, char** argv) {
   
   
    std::vector<Color> file_colors = readColorsFromFile("colors.txt");
    
    std::vector<Color> all_colors;
  
        all_colors.reserve(file_colors.size() + cli_colors.size());
        all_colors.insert(all_colors.end(), file_colors.begin(), file_colors.end());
        all_colors.insert(all_colors.end(), cli_colors.begin(), cli_colors.end());

 
        if (all_colors.empty()) {
            std::cout << "No valid colors provided." << std::endl;
            return 1;
        }

        Color result;
        if (mode == "mix") {
            result = mixColors(all_colors);
        }
        else if (mode == "lowest") {
            result = lowestColors(all_colors);
        }
        else if (mode == "highest") {
            result = highestColors(all_colors);
        }
        else if (mode == "mix-saturate") {
            result = mixSaturateColors(all_colors);
        }
        else {
            std::cerr << "Invalid mode specified, using 'mix' as default." << std::endl;
            result = mixColors(all_colors);
        }

        float hue, saturation, lightness;
        rgbToHsl(result, hue, saturation, lightness);

        std::cout << "Red: " << result.red << std::endl;
        std::cout << "Green: " << result.green << std::endl;
        std::cout << "Blue: " << result.blue << std::endl;
        std::cout << "Alpha: " << result.alpha << std::endl;
        std::cout << "Hex: #" << std::hex << result.red << result.green << result.blue << result.alpha << std::endl;
        std::cout << "Hue: " << hue * 360 << std::endl;
        std::cout << "Saturation: " << saturation << std::endl;
        std::cout << "Lightness: " << lightness << std::endl;
    
    return 0;
}
////////////////////////////
