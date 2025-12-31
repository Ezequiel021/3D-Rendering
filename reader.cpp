#include <bits/stdc++.h>

int main()
{
    std::string file_name = "cube.obj";
    std::fstream file(file_name);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file " << file_name << "\n";
    }

    std::vector<float> vertices;
    std::vector<int> indices;
    std::vector<int> faceOffset = {0};
    std::vector<int> faceSize;

    std::string input_buffer;
    std::istringstream input_buffer_stream(input_buffer);

    float f_buffer;
    int i_buffer;

    while (file >> input_buffer)
    {
        if (input_buffer == "#")
        {
            std::getline(file, input_buffer);
        }

        if (input_buffer == "v")
        {
            std::getline(file, input_buffer);
            input_buffer_stream = std::istringstream(input_buffer);
            while(input_buffer_stream >> f_buffer)
            {
                vertices.push_back(f_buffer);
            }
        }

        if (input_buffer == "f")
        {
            std::getline(file, input_buffer);

            std::cout << input_buffer << "\n";

            int face_vertex_count = 0;
            while (input_buffer_stream >> i_buffer)
            {
                ++face_vertex_count;
                indices.push_back(i_buffer);
            }
            faceSize.push_back(face_vertex_count);
            faceOffset.push_back(faceOffset[faceOffset.size() - 1] + face_vertex_count);
        }
    }

    file.close();
}